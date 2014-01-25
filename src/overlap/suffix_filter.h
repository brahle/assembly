#ifndef OVERLAP_SUFFIX_FILTER_H_
#define OVERLAP_SUFFIX_FILTER_H_

#include <stdint.h>
#include <sys/types.h>
#include <vector>

#include "util.h"


namespace overlap {


class FMIndex;
class OverlapSet;
class Read;

class SuffixFilter {
 public:
  SuffixFilter(
      const FMIndex& fmi,
      const UintArray& read_order,
      double error_rate,
      size_t min_overlap_size);

  virtual ~SuffixFilter();

  virtual OverlapSet* FindCandidates(const Read& read) const = 0;

  static size_t FactorSize(double error_rate, size_t min_overlap_size);

 protected:
  const FMIndex& fmi_;
  const UintArray& read_order_;
  const size_t min_overlap_size_;
  const size_t factor_size_;
};


class BFSSuffixFilter : public SuffixFilter {
 public:
  BFSSuffixFilter(
      const FMIndex& fmi,
      const UintArray& read_order,
      double error_rate,
      size_t min_overlap_size);

  ~BFSSuffixFilter();

  OverlapSet* FindCandidates(const Read& read);

 private:
  typedef std::tuple<uint32_t, uint32_t, uint32_t> State;

  struct state_hash : public std::unary_function<State, std::size_t> {
    std::size_t operator()(const State& k) const;
  };

  struct state_equal : public std::binary_function<State, State, bool> {
    bool operator()(const State& lhs, const State& rhs) const;
  };

  typedef std::queue<State> BFSQueue;
  typedef std::unordered_map<State, uint32_t, state_hash, state_equal> BFSMap;

  void BFS(
      const Read& read,
      uint32_t start_pos,
      uint32_t max_error);

  void CheckOverlaps(
      uint32_t low,
      uint32_t high,
      uint32_t start,
      uint32_t pos,
      size_t read_size);

  void Queue(
      uint32_t low,
      uint32_t high,
      uint32_t pos,
      uint32_t error,
      BFSQueue& queue);

  BFSMap state_dist_;
  OverlapSet* overlaps_;

  DISALLOW_COPY_AND_ASSIGN(BFSSuffixFilter);
};

}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
