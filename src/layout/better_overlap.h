#ifndef LAYOUT_BETTER_OVERLAP_H_
#define LAYOUT_BETTER_OVERLAP_H_

#include <overlap/read.h>
#include <overlap/overlap.h>
#include <cassert>
#include <vector>

namespace layout {

const double EPSILON = 0.15;
const double ALPHA = 3;

inline bool eq(double x, double y, double eps);

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
  const uint32_t Other(uint32_t read) const;

 private:
  OverlapPtr overlap_;
  ReadSetPtr read_set_;
};

class BetterOverlapSet {
  typedef BetterOverlap* BetterOverlapPtr;
 public:
  explicit BetterOverlapSet(overlap::ReadSet* read_set);
  BetterOverlapSet(
      overlap::ReadSet* read_set,
      overlap::OverlapSet* overlap_set);
  virtual ~BetterOverlapSet();

  const overlap::ReadSet* readSet() const { return read_set_; }

  const size_t size() const { return overlaps_.size(); }
  BetterOverlapPtr& operator[](size_t i)             { return overlaps_[i]; }
  const BetterOverlapPtr& operator[](size_t i) const { return overlaps_[i]; }

  void Add(BetterOverlapPtr overlap);
  void Add(overlap::Overlap* overlap);

 private:
  overlap::ReadSet* read_set_;
  std::vector< BetterOverlapPtr > overlaps_;
};

};  // namespace layout

#endif  // LAYOUT_BETTER_OVERLAP_H_
