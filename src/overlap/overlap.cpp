#include "overlap.h"


namespace overlap {


Overlap::Overlap(
    uint32_t r1,
    uint32_t r2,
    uint32_t l1,
    uint32_t l2,
    Type t,
    int32_t s)
    : read_one(r1),
      read_two(r2),
      len_one(l1),
      len_two(l2),
      type(t),
      score(s) {}

OverlapSet::OverlapSet(size_t capacity) {
  overlaps_.reserve(capacity);
}

OverlapSet::~OverlapSet() {
  for (Overlap* o : overlaps_) {
    delete o;
  }
}

void OverlapSet::Add(Overlap* overlap) {
 overlaps_.push_back(overlap);
}

size_t OverlapSet::size() const {
  return overlaps_.size();
}

Overlap* const& OverlapSet::operator[](uint32_t idx) const {
  return overlaps_[idx];
}

}  // namespace overlap
