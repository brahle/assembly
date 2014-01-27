#ifndef OVERLAP_OVERLAP_H_
#define OVERLAP_OVERLAP_H_

#include <sys/types.h>
#include <cstdint>
#include <functional>
#include <vector>

#include "util.h"


namespace overlap {


struct Overlap {
  enum Type { EB, BE, BB, EE };

  Overlap(uint32_t r1, uint32_t r2, uint32_t l1, uint32_t l2, Type t, int32_t s);

  bool operator<(const Overlap& rhs) const;

  const uint32_t read_one;
  const uint32_t read_two;
  const uint32_t len_one;
  const uint32_t len_two;
  const Type type;
  const int32_t score;
};

class OverlapCmp : public std::binary_function<Overlap*, Overlap*, bool> {
 public:
  bool operator()(const Overlap* lhs, const Overlap* rhs) const;
};

class OverlapSet {
 public:
  OverlapSet(size_t capacity);
  virtual ~OverlapSet();

  void Add(Overlap* overlap);
  void Sort();

  size_t size() const;

  Overlap* const& operator[](uint32_t idx) const;

 private:
  std::vector<Overlap*> overlaps_;

  DISALLOW_COPY_AND_ASSIGN(OverlapSet);
};


}  // namespace overlap

#endif  // OVERLAP_OVERLAP_H_
