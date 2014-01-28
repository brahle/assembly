#include "layout/better_overlap.h"

namespace layout {

BetterOverlap::BetterOverlap() : BetterOverlap(nullptr, nullptr) {
}

BetterOverlap::BetterOverlap(
    OverlapPtr overlap,
    ReadSetPtr read_set) :
    overlap_(overlap),
    read_set_(read_set) {
}

BetterOverlap::BetterOverlap(BetterOverlap* other) :
    overlap_(other->overlap_),
    read_set_(other->read_set_) {
}

BetterOverlap::~BetterOverlap() {
}

const overlap::Read* BetterOverlap::one() const {
  return read_set_->Get(overlap_->read_one);
}

const overlap::Read* BetterOverlap::two() const {
  return read_set_->Get(overlap_->read_two);
}

const uint32_t BetterOverlap::Hang(uint32_t read) const {
  assert(overlap_ != nullptr);
  assert(read_set_ != nullptr);
  assert(read == overlap_->read_one || read == overlap_->read_two);
  if (read == overlap_->read_one) {
    return read_set_->Get(overlap_->read_one)->size() - overlap_->len_one;
  }
  return read_set_->Get(overlap_->read_two)->size() - overlap_->len_two;
}

const uint32_t BetterOverlap::Length() const {
  assert(overlap_ != nullptr);
  return (overlap_->len_one + overlap_->len_two) / 2;
}

const uint32_t BetterOverlap::Length(uint32_t read) const {
  assert(overlap_ != nullptr);
  assert(read == overlap_->read_one || read == overlap_->read_two);
  if (read == overlap_->read_one) {
    return overlap_->len_one;
  }
  return overlap_->len_two;
}

const uint32_t BetterOverlap::Suf(uint32_t read) const {
  assert(overlap_ != nullptr);
  assert(read == overlap_->read_one || read == overlap_->read_two);
  if (read == overlap_->read_one) {
    return
        overlap_->type == overlap::Overlap::Type::EB ||
        overlap_->type == overlap::Overlap::Type::EE;
  }
  return
      overlap_->type == overlap::Overlap::Type::BE ||
      overlap_->type == overlap::Overlap::Type::EE;
}

const uint32_t BetterOverlap::Other(uint32_t read) const {
  assert(overlap_ != nullptr);
  assert(read == overlap_->read_one || read == overlap_->read_two);
  if (read == overlap_->read_one) {
    return overlap_->read_two;
  }
  return overlap_->read_one;
}

BetterOverlapSet::BetterOverlapSet(overlap::ReadSet* read_set) :
    read_set_(read_set),
    overlaps_() {
}

BetterOverlapSet::BetterOverlapSet(
    overlap::ReadSet* read_set,
    overlap::OverlapSet* overlap_set) :
    read_set_(read_set),
    overlaps_() {
  for (size_t i = 0; i < overlap_set->size(); ++i) {
    overlaps_.push_back(new BetterOverlap((*overlap_set)[i], read_set_));
  }
}

BetterOverlapSet::~BetterOverlapSet() {
  for (size_t i = 0; i < overlaps_.size(); ++i) {
    delete overlaps_[i];
  }
  overlaps_.clear();
}

void BetterOverlapSet::Add(BetterOverlapPtr overlap) {
  overlaps_.emplace_back(overlap);
}

void BetterOverlapSet::Add(overlap::Overlap* overlap) {
  overlaps_.emplace_back(new BetterOverlap(overlap, read_set_));
}

};  // namespace layout
