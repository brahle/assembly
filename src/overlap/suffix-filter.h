#ifndef OVERLAP_SUFFIX_FILTER_H_
#define OVERLAP_SUFFIX_FILTER_H_

#include <stdint.h>
#include <sys/types.h>
#include <queue>
#include <unordered_map>

#include "util.h"

namespace overlap {

class FmIndex;
class OverlapSet;
class Read;
class ReadSet;

// Interface for suffix filter algorithm.
class SuffixFilter {
 public:
  SuffixFilter();
  virtual ~SuffixFilter();

  // Find overlap candidates for all read.
  virtual OverlapSet* FindCandidates(
      const ReadSet& reads,
      const UintArray& read_order,
      const FmIndex& fmi) = 0;

  // Filter out candidates that are not needed, like between same reads, or
  // very similar overlaps, keeping the best ones only.
  virtual OverlapSet* FilterCandidates(
      const OverlapSet& candidates) = 0;

  // Determine factor size based on error rate and overlap size threshold.
  static size_t FactorSize(
      double error_rate,
      size_t min_overlap_size);

 protected:
  // Factor size determined with command line flags.
  const size_t factor_size_;
};

// Suffix filter that uses BFS as it's search algorithm.
class BFSSuffixFilter : public SuffixFilter {
 public:
  BFSSuffixFilter();
  ~BFSSuffixFilter();

  OverlapSet* FindCandidates(
      const ReadSet& reads,
      const UintArray& read_order,
      const FmIndex& fmi);

  OverlapSet* FilterCandidates(
      const OverlapSet& candidates);

 private:
  class BFSContext {
   public:
    BFSContext(
        const Read& read,
        const UintArray& read_order,
        const FmIndex& fmi,
        size_t factor_size,
        OverlapSet* results);

    void Start(
        uint32_t start_pos,
        uint32_t max_error);

    void Clear();

   private:
    struct State {
      uint32_t low;
      uint32_t high;
      uint32_t pos;
    };

    struct StateHash {
      std::size_t operator()(const State& k) const {
        register size_t state;
        state = ((k.low << 5) + k.low) ^ k.high;
        return ((state << 5) + state) ^ k.pos;
      }
    };

    struct StateEqual {
      bool operator()(const State& lhs, const State& rhs) const {
        return (lhs.low == rhs.low and
                lhs.high == rhs.high and
                lhs.pos == lhs.pos);
      }
    };

    typedef std::queue<State> StateQueue;
    typedef std::unordered_map<State, uint32_t, StateHash, StateEqual> StateMap;

    inline void CheckOverlaps(
        const State& state,
        uint32_t start,
        uint32_t error);

    void Queue(
        uint32_t low,
        uint32_t high,
        uint32_t pos,
        uint32_t error,
        StateQueue& queue,
        bool can_inc);

    const Read& read_;
    const UintArray& read_order_;
    const FmIndex& fmi_;
    const size_t factor_size_;

    OverlapSet* results_;

    StateQueue queue_[2];
    StateMap states_;
  };
};


}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
