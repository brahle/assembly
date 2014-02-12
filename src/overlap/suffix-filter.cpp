#include <cassert>
#include <cmath>
#include <cstdio>
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

DECLARE_int32(error_rate);
DEFINE_int32(min_overlap_size, 50, "");

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

size_t SuffixFilter::FactorSize(int32_t error_rate, size_t min_overlap_size) {
  assert(error_rate > 0);
  assert(min_overlap_size > 0);
  size_t final_size = 1000;
  for(size_t size = min_overlap_size; size < 1000; ++size) {
    size_t new_size = (size_t)(ceil(size / (ceil(0.01 * error_rate * size) + 1)) + 1e-9);
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
    const UintArray& read_order, const MyFmIndex& fmi) {

  const size_t num_reads = reads.size();
  std::unique_ptr<OverlapSet> candidates(
      new OverlapSet(num_reads * num_reads / 2));

  for (uint32_t read_idx = 0; read_idx < num_reads; ++read_idx) {
    const Read& read = *reads[read_idx];
    const size_t read_size = read.size();

    for (int32_t pos = read_size - 1;
         pos + 1 >= (int32_t)factor_size_;
         pos -= factor_size_) {

      std::unique_ptr<BFSContext> bfs(
        new BFSContext(read, read_order, fmi, factor_size_, (uint32_t)pos, candidates.get()));

      bfs->Start(0);
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

BFSSuffixFilter::BFSContext::BFSContext(const Read& read,
    const UintArray& read_order, const MyFmIndex& fmi,
    const size_t factor_size, const uint32_t start_pos, OverlapSet* results)
    : read_(read),
      read_order_(read_order),
      fmi_(fmi),
      factor_size_(factor_size),
      start_pos_(start_pos),
      results_(results),
      states_(read.size() * 3) {
}

void BFSSuffixFilter::BFSContext::Start(uint32_t start_error) {
  assert(start_pos_ < read_.size());

  State start_state = {0, (uint32_t)fmi_.size(), 0, start_error};
  Queue(queue_[0], start_state, start_error);

  for (size_t qid = 0; !queue_[qid].empty(); qid = 1 - qid) {
    StateQueue& curr_queue = queue_[qid];
    StateQueue& next_queue = queue_[1 - qid];

    for (; !curr_queue.empty(); curr_queue.pop()) {
      const State& curr_state = curr_queue.front();

      if (start_error + (curr_state.pos / factor_size_) == curr_state.error &&
          curr_state.high - curr_state.low <= 1) {
        continue;
      }

      CheckOverlaps(curr_state);

/*
    printf("%d %d %d %d %d %d %d\n",
        start_pos_, curr_state.high - curr_state.low,
        curr_state.low, curr_state.high, curr_state.pos, curr_state.error,
        read_[start_pos_ - curr_state.pos]);
        */

      if (curr_state.error > 0 && curr_state.pos <= start_pos_) {
        State next_state = {curr_state.low, curr_state.high, curr_state.pos + 1, 0};
        uint32_t next_error = curr_state.error + (next_state.pos % factor_size_ == 0 ? 1 : 0);
        Queue(next_queue, next_state, next_error - 1);
      }

      uint32_t newlow, newhigh;
      for (uint8_t cix = 1; cix <= fmi_.max_val(); ++cix) {
        newlow = fmi_.Less(cix) + fmi_.Rank(cix, curr_state.low);
        newhigh = fmi_.Less(cix) + fmi_.Rank(cix, curr_state.high);

        if (newlow >= newhigh) continue;

        if (curr_state.pos <= start_pos_) {
          State next_state = {newlow, newhigh, curr_state.pos + 1, 0};
          uint8_t next_error = curr_state.error + (next_state.pos % factor_size_ == 0 ? 1 : 0);

          if (cix == read_[start_pos_ - curr_state.pos]) {
            Queue(curr_queue, next_state, next_error);
          } else if (curr_state.error > 0) {
            Queue(next_queue, next_state, next_error - 1);
          }
        }
        if (curr_state.error > 0) {
          State next_state = {newlow, newhigh, curr_state.pos, 0};
          Queue(next_queue, next_state, curr_state.error - 1);
        }
      }
    }
  }
}

void BFSSuffixFilter::BFSContext::Clear() {
  assert(queue_[0].empty() and queue_[1].empty());
  states_.clear();
}

void BFSSuffixFilter::BFSContext::CheckOverlaps(const State& state) {
  size_t overlap_size = state.pos + read_.size() - start_pos_ - 1;
  if (state.pos >= factor_size_ && (int)overlap_size >= FLAGS_min_overlap_size) {
    uint32_t low = fmi_.Rank(0, state.low);
    uint32_t high = fmi_.Rank(0, state.high);

    for (uint32_t idx = low; idx < high; ++idx) {
      results_->Add(
          new Overlap(read_.id(), read_order_[idx], Overlap::Type::EB,
                      overlap_size, overlap_size, state.error));
    }
  }
}

void BFSSuffixFilter::BFSContext::Queue(StateQueue& queue, State& state, uint32_t error) {
  state.error = error;
  auto prev = states_.find(state);
  if (prev == states_.end() || prev->error < error) {
    queue.push(state);
    states_.insert(state);
  }
}

}  // namespace overlap
