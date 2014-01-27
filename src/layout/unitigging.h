#ifndef LAYOUT_UNITIGGING_H_
#define LAYOUT_UNITIGGING_H_

#include <overlap/read.h>
#include <overlap/overlap.h>

#include <layout/better_overlap.h>
#include <layout/better_read.h>
#include <layout/contig.h>

namespace layout {

class Unitigging {
 public:
  Unitigging(
      overlap::ReadSet* reads,
      overlap::OverlapSet* overlaps);
  virtual ~Unitigging();

  void start();

 private:
  overlap::ReadSet* reads_;
  overlap::OverlapSet* orig_overlaps_;
  BetterOverlapSet overlaps_;
  BetterOverlapSet* no_contains_;
  BetterOverlapSet* no_transitives_;
  ContigSet* contigs_;

  void removeContainmentEdges();
  bool isTransitive(
      BetterOverlap* o1,
      BetterOverlap* o2,
      BetterOverlap* o3) const;
  void removeTransitiveEdges();
  void makeContigs();
};

}  // namespace layout

#endif  // LAYOUT_UNITIGGING_H_
