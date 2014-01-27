#ifndef LAYOUT_BETTER_READ_H_
#define LAYOUT_BETTER_READ_H_

#include <overlap/read.h>
#include <layout/better_overlap.h>
#include <vector>
#include <utility>

namespace layout {

class BetterRead {
  typedef overlap::Read* ReadPtr;
  typedef layout::BetterOverlap* BetterOverlapPtr;
  typedef std::vector< std::pair< uint32_t, BetterOverlapPtr > > OverlapVector;

 public:
  explicit BetterRead(uint32_t id);
  BetterRead(uint32_t id, ReadPtr read);
  virtual ~BetterRead();

  const ReadPtr read() const { return read_; }
  const uint32_t id() const { return id_; }

  void AddOverlap(BetterOverlapPtr overlap);
  void Finalize();
  const OverlapVector& overlaps() const;

 private:
  uint32_t id_;
  ReadPtr read_;
  OverlapVector overlaps_;
  bool finalized_;
};

class BetterReadSet {
  typedef overlap::ReadSet* ReadSetPtr;
  typedef BetterRead* BetterReadPtr;
 public:
  explicit BetterReadSet(size_t size);
  BetterReadSet(ReadSetPtr read_set, bool master);
  virtual ~BetterReadSet();

  BetterReadPtr& operator[](size_t idx);
  const BetterReadPtr& operator[](size_t idx) const;
  const size_t size() const;
  void Finalize();

 private:
  std::vector< BetterReadPtr > read_set_;
  bool master_;
};

};  // namespace layout

#endif  // LAYOUT_BETTER_READ_H_
