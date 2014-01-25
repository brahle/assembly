#include <cmath>
#include <memory>
#include <queue>
#include <tuple>
#include <unordered_map>

#include "fm_index.h"
#include "overlap.h"
#include "read.h"
#include "suffix_filter.h"
#include "util.h"


namespace overlap {


SuffixFilter::SuffixFilter(
    const FMIndex& fm_index,
    const UintArray& read_order,
    double error_rate,
    size_t min_overlap_size)
    : fm_index_(fm_index),
      read_order_(read_order),
      min_overlap_size_(min_overlap_size),
      factor_size_(SuffixFilter::FactorSize(error_rate, min_overlap_size)) {
}

SuffixFilter::~SuffixFilter() {
}

size_t SuffixFilter::FactorSize(double error_rate, size_t min_overlap_size) {
  size_t final_size = 1000;
  for(size_t size = min_overlap_size; size < 1000; ++size) {
    size_t new_size = (size_t)(ceil(size / ceil(error_rate * size) + 1) + 1e-9);
    final_size = std::min(final_size, new_size);
  }
  return final_size;
}

BFSSuffixFilter::BFSSuffixFilter(
    const FMIndex& fm_index,
    const UintArray& read_order,
    double error_rate,
    size_t min_overlap_size)
    : SuffixFilter(fm_index, read_order, error_rate, min_overlap_size) {
}

BFSSuffixFilter::~BFSSuffixFilter() {
}

OverlapSet* BFSSuffixFilter::FindCandidates(const Read& read) const {
  if (read.size() < min_overlap_size_) {
    return nullptr;
  }

  const size_t read_size = read.size();
  std::unique_ptr<OverlapSet> overlaps(new OverlapSet(1 << 10));

  for (uint32_t pos = read_size - 1; pos + 1 >= factor_size_; pos -= factor_size_) {
    BFS(read, pos, ((size_t)pos == read_size - 1 ? 1 : 0), overlaps.get());
  }

  return overlaps.release();
}

namespace {

typedef std::tuple<uint32_t, uint32_t, uint32_t> State;

struct state_hash : public std::unary_function<State, std::size_t> {
  std::size_t operator()(const State& k) const {
    return (std::hash<uint32_t>()(std::get<0>(k)) ^
            std::hash<uint32_t>()(std::get<1>(k)) ^
            std::hash<uint32_t>()(std::get<2>(k)));
  }
};

struct state_equal : public std::binary_function<State, State, bool> {
  bool operator()(const State& lhs, const State& rhs) const {
    return (std::get<0>(lhs) == std::get<0>(rhs) &&
            std::get<1>(lhs) == std::get<1>(rhs) &&
            std::get<2>(lhs) == std::get<2>(rhs));
  }
};

}

void BFSSuffixFilter::BFS(
    const Read& read,
    size_t start_pos,
    size_t max_error,
    OverlapSet* overlaps) const {

  if (read.size() && overlaps) {
    overlaps = nullptr;
    start_pos -= max_error;
  }

  std::queue<State> Q[2];
  std::unordered_map<State, uint32_t, state_hash, state_equal> E;

  State start_state = std::make_tuple(0, fm_index_.size(), 0);

  Q[0].push(start_state);
  E[start_state] = max_error;

  for (size_t qid = 0; !Q[qid].empty(); qid = 1 - qid) {
    std::queue<State>& curr = Q[qid];
    std::queue<State>& next = Q[1 - qid];

    while(!curr.empty()) {
      next.push(std::make_tuple(1, 2, 3));
      curr.pop();
    }
  }
}

}  // namespace overlap
