#include <cassert>
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
    const FMIndex& fmi,
    const UintArray& read_order,
    double error_rate,
    size_t min_overlap_size)
    : fmi_(fmi),
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
    const FMIndex& fmi,
    const UintArray& read_order,
    double error_rate,
    size_t min_overlap_size)
    : SuffixFilter(fmi, read_order, error_rate, min_overlap_size) {
}

BFSSuffixFilter::~BFSSuffixFilter() {
}

void BFSSuffixFilter::FindCandidates(
    const Read& read,
    OverlapSet* overlaps) {
  if (read.size() < min_overlap_size_) {
    return;
  }

  const size_t read_size = read.size();
  overlaps_ = overlaps;

  for (int32_t pos = read_size - 1; pos + 1 >= (int32_t)factor_size_; pos -= factor_size_) {
    BFS(read, pos, ((size_t)pos == read_size - 1 ? 1 : 0));
  }
}

OverlapSet* BFSSuffixFilter::FilterContained(
    OverlapSet* overlaps) {
  if (overlaps) {
    return nullptr;
  }
  return nullptr;
}

std::size_t BFSSuffixFilter::state_hash::operator()(const State& k) const {
  return (std::hash<uint32_t>()(std::get<0>(k)) ^
          std::hash<uint32_t>()(std::get<1>(k)) ^
          std::hash<uint32_t>()(std::get<2>(k)));
}

bool BFSSuffixFilter::state_equal::operator()(const State& lhs, const State& rhs) const {
  return (std::get<0>(lhs) == std::get<0>(rhs) &&
          std::get<1>(lhs) == std::get<1>(rhs) &&
          std::get<2>(lhs) == std::get<2>(rhs));
}

void BFSSuffixFilter::BFS(
    const Read& read,
    uint32_t start_pos,
    uint32_t max_error) {

  BFSQueue queues[2];
  state_dist_.clear();

  State start_state = std::make_tuple(0, fmi_.size(), 0);
  queues[0].push(start_state);
  state_dist_[start_state] = max_error;

  for (size_t qid = 0; !queues[qid].empty(); qid = 1 - qid) {
    std::queue<State>& curr = queues[qid];
    std::queue<State>& next = queues[1 - qid];

    uint32_t low, high, pos, error, value;
    while(!curr.empty()) {
      std::tie(low, high, pos) = curr.front();
      error = state_dist_[curr.front()];

      // printf("%d, %d %d, %d %d: %d\n", read.id(), low, high, start_pos, pos, error);

      CheckOverlaps(read.id(), low, high, start_pos, pos, error, read.size());

      if (error > 0 && pos <= start_pos) {
        Queue(low, high, pos + 1, error - 1, next, true);
      }

      uint32_t newlow, newhigh;
      for (uint8_t cix = 1; cix <= fmi_.max_val(); ++cix) {
        newlow = fmi_.Less(cix) + fmi_.Rank(cix, low);
        newhigh = fmi_.Less(cix) + fmi_.Rank(cix, high);
        if (newlow >= newhigh) continue;

        if (pos <= start_pos) {
          value = read[start_pos - pos];
          if (cix == value) {
            Queue(newlow, newhigh, pos + 1, error, curr, true);
          } else if (error > 0) {
            Queue(newlow, newhigh, pos + 1, error - 1, next, true);
          }
        } else if (error > 0) {
          Queue(newlow, newhigh, pos, error - 1, next, false);
        }
      }

      curr.pop();
    }
  }
}

void BFSSuffixFilter::CheckOverlaps(
    uint32_t id,
    uint32_t low,
    uint32_t high,
    uint32_t start,
    uint32_t pos,
    uint32_t error,
    size_t read_size) {

  assert(low <= fmi_.size() && high <= fmi_.size());
  size_t overlap_size = pos + read_size - start - 1;
  if (pos >= factor_size_ && overlap_size >= min_overlap_size_) {
    low = fmi_.Rank(0, low);
    high = fmi_.Rank(0, high);
    assert(low <= read_order_.size() && high <= read_order_.size());
    for (uint32_t idx = low; idx < high; ++idx) {
      overlaps_->Add(
          new Overlap(id, read_order_[idx], overlap_size, overlap_size, Overlap::Type::EB, error));
    }
  }
}

void BFSSuffixFilter::Queue(
    uint32_t low,
    uint32_t high,
    uint32_t pos,
    uint32_t error,
    BFSQueue& queue,
    bool can_inc) {

  State new_state(low, high, pos);
  if (state_dist_.find(new_state) == state_dist_.end()) {
    queue.push(new_state);
    state_dist_[new_state] = error + (can_inc && !(pos % factor_size_) ? 1 : 0);
  }
}


}  // namespace overlap
