#include "layout/better_read.h"

#include <algorithm>

namespace layout {

BetterRead::BetterRead(uint32_t id) : BetterRead(id, nullptr) {
}

BetterRead::BetterRead(uint32_t id, ReadPtr read) :
    id_(id), read_(read), overlaps_(), finalized_(false) {
}

BetterRead::~BetterRead() {
  // TODO(brahle): mozda trebas zbrisati?
}

void BetterRead::AddOverlap(BetterOverlapPtr overlap) {
  overlaps_.emplace_back(overlap->Other(id_), overlap);
}

void BetterRead::Finalize() {
  std::sort(overlaps_.begin(), overlaps_.end());
  finalized_ = true;
}

const BetterRead::OverlapVector& BetterRead::overlaps() const {
  assert(finalized_);
  return overlaps_;
}


BetterReadSet::BetterReadSet(size_t size) :
    read_set_(size),
    master_(true) {
}

BetterReadSet::BetterReadSet(ReadSetPtr read_set, bool master = false) :
    read_set_(),
    master_(master) {
  for (size_t idx = 0; idx < read_set->size(); ++idx) {
    read_set_.emplace_back(new BetterRead(idx, (*read_set)[idx]));
  }
}

BetterReadSet::~BetterReadSet() {
  for (BetterReadPtr read : read_set_) {
    delete read;
  }
}

BetterReadSet::BetterReadPtr& BetterReadSet::operator[](size_t i) {
  return read_set_[i];
}

const BetterReadSet::BetterReadPtr& BetterReadSet::operator[](size_t i) const {
  return read_set_[i];
}

const size_t BetterReadSet::size() const {
  return read_set_.size();
}

void BetterReadSet::Finalize() {
  for (auto &read : read_set_) {
    read->Finalize();
  }
}

};  // namespace layout
