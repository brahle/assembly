#ifndef OVERLAP_SUFFIX_FILTER_H_
#define OVERLAP_SUFFIX_FILTER_H_

#include <stdint.h>
#include <sys/types.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <sparsehash/sparse_hash_map>

#include "util.h"

namespace overlap {

class WaveletFmIndex;
class OverlapSet;
class Read;
class ReadSet;

typedef WaveletFmIndex MyFmIndex;

// Interface for suffix filter algorithm.
class SuffixFilter {
 public:
  SuffixFilter();
  virtual ~SuffixFilter();

  // Find overlap candidates for all read.
  virtual OverlapSet* FindCandidates(
      const ReadSet& reads,
      const UintArray& read_order,
      const MyFmIndex& fmi) = 0;

  // Filter out candidates that are not needed, like between same reads, or
  // very similar overlaps, keeping the best ones only.
  virtual OverlapSet* FilterCandidates(
      const OverlapSet& candidates) = 0;

  // Determine factor size based on error rate and overlap size threshold.
  static size_t FactorSize(
      int32_t error_rate,
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
      const MyFmIndex& fmi);

  OverlapSet* FilterCandidates(
      const OverlapSet& candidates);

 private:
  class BFSContext {
   public:
    BFSContext(
        const Read& read,
        const UintArray& read_order,
        const MyFmIndex& fmi,
        const size_t factor_size,
        const uint32_t start_pos,
        OverlapSet* results);

    void Start(
        uint32_t start_error);

    void Clear();

    size_t StateMapSize() const {
      return states_.size();
    }

    struct State {
      uint32_t low;
      uint32_t high;
      uint32_t pos;
      uint32_t error;
    };

   private:
    struct StateHash {
      std::size_t operator()(const State& k) const {
        register size_t state;
        state = ((k.high << 5) + k.high) ^ k.low;
        return ((state << 5) + state) ^ k.pos;
      }
    };

    struct StateEqual {
      bool operator()(const State& lhs, const State& rhs) const {
        return (lhs.low == rhs.low and
                lhs.high == rhs.high and
                lhs.pos == rhs.pos);
      }
    };

    typedef std::queue<State> StateQueue;
    typedef std::unordered_set<State, StateHash, StateEqual> StateMap;
    //typedef google::sparse_hash_map<State, uint32_t, StateHash, StateEqual> StateMap;

    inline void CheckOverlaps(const State& state);

    void Queue(StateQueue& queue, State& state, const uint32_t error);

    const Read& read_;
    const UintArray& read_order_;
    const MyFmIndex& fmi_;
    const size_t factor_size_;
    const uint32_t start_pos_;

    OverlapSet* results_;

    StateMap states_;
    StateQueue queue_[2];
  };
};


}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
