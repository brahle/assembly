#include "unitigging.h"

namespace layout {

Unitigging::Unitigging(
    overlap::ReadSet* reads,
    overlap::OverlapSet* overlaps)
    : reads_(reads),
      orig_overlaps_(overlaps),
      overlaps_(reads, overlaps) {
}

Unitigging::~Unitigging() {}

void Unitigging::start() {
  removeContainmentEdges();
  removeTransitiveEdges();
  makeContigs();
}

void Unitigging::removeContainmentEdges() {
  bool *erased = new bool[reads_->size()];
  for (size_t i = 0; i < overlaps_.size(); ++i) {
    BetterOverlap& o = overlaps_[i];
    if (true) {
      erased[o.overlap()->read_one] = 1;
    } else if (true) {
      erased[o.overlap()->read_two] = 1;
    }
  }
  delete [] erased;
}

void Unitigging::removeTransitiveEdges() {
}

void Unitigging::makeContigs() {
}

};

