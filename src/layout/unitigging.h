#ifndef _UNITIGGING_H_
#define _UNITIGGING_H_

#include <overlap/overlap.h>
#include <overlap/read.h>

namespace layout {

class Unitigging {
 public:
  Unitigging(const overlap::ReadSet& reads);
  virtual ~Unitigging();

  void start();

 private:
  void removeContainmentEdges();
  void removeTransitiveEdges();
  void connectEdges();
  const overlap::ReadSet& reads_;
};

}
#endif /* _UNITIGGING_H_ */
