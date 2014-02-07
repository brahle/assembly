#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <gflags/gflags.h>

#include "fm-index.h"
#include "overlap.h"
#include "read.h"
#include "suffix-filter.h"
#include "util.h"

DECLARE_double(error_rate);
DEFINE_int32(min_overlap_size, 25, "");

namespace overlap {

namespace {

void FilterPass(
    const std::vector<const Overlap*>& invec,
    std::vector<const Overlap*>& outvec) {

  const Overlap* prev = nullptr;
  for (auto curr : invec) {
    if (prev == nullptr) {
      outvec.push_back(curr);
      prev = curr;
    } else if (prev->read_one == curr->read_one and
               prev->read_two == curr->read_two and
               prev->type == curr->type) {
      if (abs((int32_t)curr->len_one - (int32_t)prev->len_one) >
          prev->score - curr->score + 1) {
        outvec.push_back(curr);
        prev = curr;
      }
    } else {
      outvec.push_back(curr);
      prev = curr;
    }
  }
}

}  // unnamed namespace

SuffixFilter::SuffixFilter()
  : factor_size_(SuffixFilter::FactorSize(FLAGS_error_rate, FLAGS_min_overlap_size)) {
}

SuffixFilter::~SuffixFilter() {
}

size_t SuffixFilter::FactorSize(double error_rate, size_t min_overlap_size) {
  assert(error_rate > 0);
  assert(min_overlap_size > 0);
  size_t final_size = 1000;
  for(size_t size = min_overlap_size; size < 1000; ++size) {
    size_t new_size = (size_t)(ceil(size / ceil(error_rate * size) + 1) + 1e-9);
    final_size = std::min(final_size, new_size);
  }
  return final_size;
}

BFSSuffixFilter::BFSSuffixFilter()
    : SuffixFilter() {
}

BFSSuffixFilter::~BFSSuffixFilter() {
}

OverlapSet* BFSSuffixFilter::FindCandidates(const ReadSet& reads,
    const UintArray& read_order, const FmIndex& fmi) {

  const size_t num_reads = reads.size();
  std::unique_ptr<OverlapSet> candidates(
      new OverlapSet(num_reads * num_reads / 2));

  for (uint32_t read_idx = 0; read_idx < num_reads; ++read_idx) {
    const Read& read = *reads[read_idx];
    const size_t read_size = read.size();

    for (int32_t pos = read_size - 1;
         pos + 1 >= (int32_t)factor_size_ * 2;
         pos -= factor_size_) {

      std::unique_ptr<BFSContext> bfs(
        new BFSContext(read, read_order, fmi, factor_size_, candidates.get()));

      bfs->Start(pos, (size_t)pos == read_size - 1);
      //bfs->Clear();
    }
  }

  return candidates.release();
}

OverlapSet* BFSSuffixFilter::FilterCandidates(const OverlapSet& candidates) {
  std::vector<const Overlap*> cont;

  for (uint32_t idx = 0; idx < candidates.size(); ++idx) {
    const Overlap* curr = candidates[idx];
    if (curr->read_one != curr->read_two) {
      cont.push_back(curr);
    }
  }

  std::vector<const Overlap*> fwd, bwd;
  FilterPass(cont, fwd);

  std::reverse(fwd.begin(), fwd.end());
  FilterPass(fwd, bwd);

  std::reverse(bwd.begin(), bwd.end());
  std::unique_ptr<OverlapSet> filtered(new OverlapSet(bwd.size()));

  for (auto o : bwd) {
    filtered->Add(new Overlap(*o));
  }

  return filtered.release();
}

BFSSuffixFilter::BFSContext::BFSContext(const Read& read, const UintArray& read_order,
    const FmIndex& fmi, size_t factor_size, OverlapSet* results)
    : read_(read),
      read_order_(read_order),
      fmi_(fmi),
      factor_size_(factor_size),
      results_(results),
      states_() {
}

void BFSSuffixFilter::BFSContext::Start(uint32_t start_pos, uint32_t error) {
  assert(start_pos < read_.size());
  Queue(0, fmi_.size(), 0, error, queue_[0], false);

  for (size_t qid = 0; !queue_[qid].empty(); qid = 1 - qid) {
    std::queue<State>& curr = queue_[qid];
    std::queue<State>& next = queue_[1 - qid];

    while(!curr.empty()) {
      State& state = curr.front();
      error = states_.at(state);

      CheckOverlaps(state, start_pos, error);

      if (error > 0 && state.pos <= start_pos) {
        Queue(state.low, state.high, state.pos + 1, error - 1, next, true);
      }

      uint32_t newlow, newhigh;
      for (uint8_t cix = 1; cix <= fmi_.max_val(); ++cix) {
        newlow = fmi_.Less(cix) + fmi_.Rank(cix, state.low);
        newhigh = fmi_.Less(cix) + fmi_.Rank(cix, state.high);
        if (newlow >= newhigh) continue;

        if (state.pos <= start_pos) {
          if (cix == read_[start_pos - state.pos]) {
            Queue(newlow, newhigh, state.pos + 1, error, curr, true);
          } else if (error > 0) {
            Queue(newlow, newhigh, state.pos + 1, error - 1, next, true);
          }
        } else if (error > 0) {
          Queue(newlow, newhigh, state.pos, error - 1, next, false);
        }
      }

      curr.pop();
    }
  }
}

void BFSSuffixFilter::BFSContext::Clear() {
  assert(queue_[0].empty() and queue_[1].empty());
  states_.clear();
}

void BFSSuffixFilter::BFSContext::CheckOverlaps(
    const State& state, uint32_t start, uint32_t error) {
  size_t overlap_size = state.pos + read_.size() - start - 1;
  if (state.pos >= factor_size_ && (int)overlap_size >= FLAGS_min_overlap_size) {
    uint32_t low = fmi_.Rank(0, state.low);
    uint32_t high = fmi_.Rank(0, state.high);

    for (uint32_t idx = low; idx < high; ++idx) {
      results_->Add(
          new Overlap(read_.id(), read_order_[idx], Overlap::Type::EB,
                      overlap_size, overlap_size, error));
    }
  }
}

void BFSSuffixFilter::BFSContext::Queue(uint32_t low, uint32_t high,
    uint32_t pos, uint32_t error, StateQueue& queue, bool can_inc) {
  State new_state = {low, high, pos};
  if (states_.find(new_state) == states_.end()) {
    queue.push(new_state);
    states_.insert(StateMap::value_type(
          new_state,
          error + (can_inc && !(pos % factor_size_) ? 1 : 0)));
  }
}

}  // namespace overlap