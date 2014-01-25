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
      const FMIndex& fm_index,
      const UintArray& read_order,
      double error_rate,
      size_t min_overlap_size);

  virtual ~SuffixFilter();

  virtual OverlapSet* FindCandidates(const Read& read) const = 0;

  static size_t FactorSize(double error_rate, size_t min_overlap_size);

 protected:
  const FMIndex& fm_index_;
  const UintArray& read_order_;
  const size_t min_overlap_size_;
  const size_t factor_size_;
};


class BFSSuffixFilter : public SuffixFilter {
 public:
  BFSSuffixFilter(
      const FMIndex& fm_index,
      const UintArray& read_order,
      double error_rate,
      size_t min_overlap_size);

  ~BFSSuffixFilter();

  OverlapSet* FindCandidates(const Read& read) const;

 private:
  void BFS(
      const Read& read,
      size_t start_pos,
      size_t max_error,
      OverlapSet* overlaps) const;
};

}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
