#include "unitigging.h"

namespace layout {

Unitigging::Unitigging(
    overlap::ReadSet* reads,
    overlap::OverlapSet* overlaps)
    : reads_(reads),
      orig_overlaps_(overlaps),
      overlaps_(reads, overlaps),
      no_contains_(nullptr),
      no_transitives_(nullptr) {
}

Unitigging::~Unitigging() {
  delete no_contains_;
  delete no_transitives_;
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
  no_contains_ = new BetterOverlapSet(reads_);
  for (size_t i = 0; i < overlaps_.size(); ++i) {
    BetterOverlap* better_overlap = overlaps_[i];
    overlap::Overlap* overlap = better_overlap->overlap();
    if (erased[overlap->read_one]) continue;
    if (erased[overlap->read_two]) continue;
    no_contains_->Add(better_overlap);
  }
  delete [] erased;
}

void Unitigging::removeTransitiveEdges() {
  layout::BetterReadSet brs(reads_, 1);
  for (size_t i = 0; i < no_contains_->size(); ++i) {
    auto better_overlap = (*no_contains_)[i];
    auto overlap = better_overlap->overlap();
    brs[overlap->read_one]->AddOverlap(better_overlap);
    brs[overlap->read_two]->AddOverlap(better_overlap);
  }
  brs.Finalize();

  std::vector< int > erased;

  for (size_t i = 0; i < no_contains_->size(); ++i) {
    auto better_overlap = (*no_contains_)[i];
    auto overlap = better_overlap->overlap();

    // TODO: izdvoji ovo u zasebnu klasu iterator
    auto v1 = brs[overlap->read_one]->overlaps();
    auto v2 = brs[overlap->read_two]->overlaps();
    auto it1 = v1.begin();
    auto it2 = v2.begin();
    while (it1 != v1.end() && it2 != v2.end()) {
      if (it1->first == it2->first) {
        // potencijalno tranzitivni
        // TODO: provjeri uvjete!
        if (true) {
          erased.emplace_back(i);
        }
        ++it2;
      } else if (it1->first < it2->first) {
        ++it1;
      } else {
        ++it2;
      }
    }
  }

  no_transitives_ = new BetterOverlapSet(reads_);
  size_t idx = 0;
  for (size_t i = 0; i < no_contains_->size(); ++i) {
    if (idx < erased.size() && i == erased[idx]) {
      ++idx;
      continue;
    }
    auto better_overlap = (*no_contains_)[i];
    no_transitives_->Add(better_overlap);
  }
}

void Unitigging::makeContigs() {
}

};

