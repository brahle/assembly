#ifndef _BETTER_OVERLAP_H_
#define _BETTER_OVERLAP_H_

#include <cassert>
#include <vector>

#include <overlap/read.h>
#include <overlap/overlap.h>

namespace layout {

class BetterOverlap {
  typedef overlap::ReadSet* ReadSetPtr;
  typedef overlap::Overlap* OverlapPtr;
 public:
  BetterOverlap();
  BetterOverlap(OverlapPtr overlap, ReadSetPtr read_set);
  virtual ~BetterOverlap();

  const OverlapPtr& overlap() const { return overlap_; }
  const ReadSetPtr& readSet() const { return read_set_; }
  void setReadSet(ReadSetPtr read_set) { read_set_ = read_set; }

  const overlap::Read* one() const;
  const overlap::Read* two() const;

  const uint32_t Hang(uint32_t read) const;
  const uint32_t Length() const;
  const uint32_t Length(uint32_t read) const;
  const uint32_t Suf(uint32_t read) const;

 private:
  OverlapPtr overlap_;
  ReadSetPtr read_set_;
};

class BetterOverlapSet {
 public:
  BetterOverlapSet(
      overlap::ReadSet* read_set,
      overlap::OverlapSet* overlapSet);
  virtual ~BetterOverlapSet();

  const size_t size() const { return overlaps_.size(); }
  BetterOverlap& operator[](size_t i)             { return overlaps_[i]; }
  const BetterOverlap& operator[](size_t i) const { return overlaps_[i]; }

 private:
  overlap::ReadSet* read_set_;
  std::vector<BetterOverlap> overlaps_;
};

};

#endif /* _BETTER_OVERLAP_H_ */
