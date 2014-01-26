#include "unitigging.h"

namespace layout {

Unitigging::Unitigging(
    overlap::ReadSet* reads,
    overlap::OverlapSet* overlaps)
    : reads_(reads),
      orig_overlaps_(overlaps),
      overlaps_(reads, overlaps),
      partly_cleared_(nullptr) {
}

Unitigging::~Unitigging() {
  delete partly_cleared_;
}

void Unitigging::start() {
  removeContainmentEdges();
  removeTransitiveEdges();
  makeContigs();
}

void Unitigging::removeContainmentEdges() {
  bool *erased = new bool[reads_->size()];
  for (size_t i = 0; i < overlaps_.size(); ++i) {
    BetterOverlap* better_overlap = overlaps_[i];
    overlap::Overlap* overlap = better_overlap->overlap();
    if (better_overlap->one()->size() == overlap->len_one) {
      erased[overlap->read_one] = 1;
    } else if (better_overlap->two()->size() == overlap->len_two) {
      erased[overlap->read_two] = 1;
    }
  }
  partly_cleared_ = new BetterOverlapSet(reads_);
  for (size_t i = 0; i < overlaps_.size(); ++i) {
    BetterOverlap* better_overlap = overlaps_[i];
    overlap::Overlap* overlap = better_overlap->overlap();
    if (erased[overlap->read_one]) continue;
    if (erased[overlap->read_two]) continue;
    partly_cleared_->Add(better_overlap);
  }
  delete [] erased;
}

void Unitigging::removeTransitiveEdges() {
}

void Unitigging::makeContigs() {
}

};

