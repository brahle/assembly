#include "fm_index.h"
#include "overlap.h"
#include "read.h"
#include "suffix_filter.h"
#include "util.h"


namespace overlap {


SuffixFilter::SuffixFilter(
    double error_rate,
    size_t min_overlap_size)
    : error_rate_(error_rate),
      min_overlap_size_(min_overlap_size) {
}

SuffixFilter::~SuffixFilter() {
}

BFSSuffixFilter::BFSSuffixFilter(
    double error_rate,
    size_t min_overlap_size)
  : SuffixFilter(error_rate, min_overlap_size) {
}

BFSSuffixFilter::~BFSSuffixFilter() {
}

OverlapSet* BFSSuffixFilter::FindCandidates(
    const ReadSet& read_set,
    const FMIndex& fm_index,
    const IntArray& read_order) const {

  if (read_set.size() > 1) {
    if (fm_index.size() > 2) {
      if (read_order.size() > 5)
        return new OverlapSet(1024);
    }
  }

  return nullptr;
}


}  // namespace overlap
