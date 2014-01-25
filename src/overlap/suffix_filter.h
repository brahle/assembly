#ifndef OVERLAP_SUFFIX_FILTER_H_
#define OVERLAP_SUFFIX_FILTER_H_

#include <stdint.h>
#include <sys/types.h>
#include <vector>

#include "util.hpp"


namespace overlap {


class FMIndex;
class OverlapSet;
class ReadSet;

class SuffixFilter {
 public:
  SuffixFilter(double error_rate, size_t min_overlap_size);
  virtual ~SuffixFilter();

  virtual OverlapSet* FindCandidates(
      const ReadSet& read_set,
      const FMIndex& fm_index,
      const IntArray& read_order) const = 0;

 protected:
  const double error_rate_;
  const size_t min_overlap_size_;
};


class BFSSuffixFilter : public SuffixFilter {
 public:
  BFSSuffixFilter(double error_rate, size_t min_overlap_size);
  ~BFSSuffixFilter();

  OverlapSet* FindCandidates(
      const ReadSet& read_set,
      const FMIndex& fm_index,
      const IntArray& read_order) const;
};

}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
