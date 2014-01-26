#ifndef _UNITIGGING_H_
#define _UNITIGGING_H_

#include <overlap/read.h>
#include <overlap/overlap.h>

#include <layout/better_overlap.h>

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
  layout::BetterOverlapSet overlaps_;

  void removeContainmentEdges();
  void removeTransitiveEdges();
  void makeContigs();
};

}
#endif /* _UNITIGGING_H_ */
