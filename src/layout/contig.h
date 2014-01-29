#ifndef LAYOUT_CONTIG_H_
#define LAYOUT_CONTIG_H_

#include <layout/better_overlap.h>
#include <layout/better_read.h>
#include <deque>
#include <vector>

namespace layout {

class Contig {
  typedef BetterRead* BetterReadPtr;
  typedef BetterReadSet* BetterReadSetPtr;
  typedef BetterOverlap* BetterOverlapPtr;
 public:
  Contig(BetterReadPtr starting, BetterReadSetPtr read_set);
  virtual ~Contig();

  size_t size() const;
  void Join(BetterOverlapPtr better_overlap, Contig* contig);

 private:
  std::deque< BetterReadPtr > reads_;
  BetterReadSetPtr read_set_;
  bool alive_;
  bool IsLeftOverlap(BetterOverlapPtr better_overlap) const;
  void Kill();
};

class ContigSet {
  typedef Contig* ContigPtr;
 public:
  explicit ContigSet(BetterReadSet* read_set);
  virtual ~ContigSet();

  size_t size() const;
  ContigPtr& operator[](int i);
  const ContigPtr& operator[](int i) const;

 private:
  std::vector< ContigPtr > contigs_;
};

}  // namespace layout

#endif  // LAYOUT_CONTIG_H_
