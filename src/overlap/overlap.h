#ifndef OVERLAP_OVERLAP_H_
#define OVERLAP_OVERLAP_H_

#include <sys/types.h>
#include <cstdint>
#include <functional>
#include <vector>

#include "util.h"

namespace overlap {

// Represents a potential overlap between two DNA reads.
struct Overlap {
  // Describes at what ends do these reads overlap.
  enum Type {
    EB,  // End (E) of first with beginning (B) of second.
    BB,  // Both at beginnings, one is reverse-complemented.
    EE,  // Both at ends, one is reverse-complemented.
    BE   // Beginning of first with end of second.
  };

  // Constructs a new Overlap given the values.
  Overlap(
      uint32_t read1,
      uint32_t read2,
      Type t,
      uint32_t len1,
      uint32_t len2,
      int32_t s);

  // Compares overlaps in the following order:
  //  read_one ASC
  //  read_two ASC
  //  type ASC
  //  len_one ASC
  //  len_two ASC
  //  score DESC
  bool operator<(const Overlap& rhs) const;

  // Internal read IDs.
  uint32_t read_one;
  uint32_t read_two;
  // Overlap type.
  Type type;
  // Overlap lengths at each read.
  uint32_t len_one;
  uint32_t len_two;
  // Edit distance, confidence or something else.
  int32_t score;
};

// Comparator that compares Overlap* according to its operator<.
class OverlapCmp : public std::binary_function<Overlap*, Overlap*, bool> {
 public:
  bool operator()(const Overlap* lhs, const Overlap* rhs) const;
};

// Sequential container for overlaps.
class OverlapSet {
 public:
  // Create a new OverlapSet given the approx. number of overlaps to be added.
  OverlapSet(size_t capacity);
  virtual ~OverlapSet();

  // Return the number of contained reads.
  size_t size() const;

  // Sort the overlaps in-place.
  void Sort();
  // Add the overlap to the end and take it's ownership.
  void Add(Overlap* overlap);

  // Return the read at position 'idx'.
  const Overlap* Get(uint32_t idx) const;
  Overlap* Get(uint32_t idx);
  const Overlap* operator[](uint32_t idx) const;
  Overlap* operator[](uint32_t idx);

 private:
  // Contained overlaps. Owned.
  std::vector<Overlap*> overlaps_;

  DISALLOW_COPY_AND_ASSIGN(OverlapSet);
};


}  // namespace overlap

#endif  // OVERLAP_OVERLAP_H_
