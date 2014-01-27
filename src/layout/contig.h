#ifndef _CONTIG_H_
#define _CONTIG_H_

#include <deque>

#include <layout/better_overlap.h>
#include <layout/better_read.h>

namespace layout {

class Contig {
  typedef BetterRead* BetterReadPtr;
  typedef BetterReadSet* BetterReadSetPtr;
  typedef BetterOverlap* BetterOverlapPtr;
 public:
  Contig(BetterReadPtr starting, BetterReadSetPtr read_set);
  virtual ~Contig();

  void Join(BetterOverlapPtr better_overlap, Contig* contig);

 private:
  std::deque< BetterReadPtr > reads_;
  BetterReadSetPtr read_set_;
  bool alive_;
  bool IsLeftOverlap(BetterOverlapPtr better_overlap) const;
  void Kill();
};

class ContigSet
{
  typedef Contig* ContigPtr;
 public:
  ContigSet(BetterReadSet* read_set);
  virtual ~ContigSet();
 private:
  std::vector< ContigPtr > contigs_;
};

}

#endif /* _CONTIG_H_ */
