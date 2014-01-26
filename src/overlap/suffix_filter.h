#ifndef OVERLAP_SUFFIX_FILTER_H_
#define OVERLAP_SUFFIX_FILTER_H_

#include <cstdint>
#include <vector>

#include "util.hpp"


namespace overlap {


class FMIndex;
class OverlapSet;
class ReadSet;

class SuffixFilter {
 public:
  SuffixFilter(double error_rate);
  virtual ~SuffixFilter() = 0;

  virtual OverlapSet* FindCandidates(
      const ReadSet& read_set,
      const FMIndex& fm_index,
      const IntArray& read_order) = 0;

 private:
  const double error_rate_;
};


}  // namespace overlap

#endif  // OVERLAP_SUFFIX_FILTER_H_
