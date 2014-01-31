#ifndef OVERLAP_SORT_H_
#define OVERLAP_SORT_H_

#include "util.h"

namespace overlap {

class ReadSet;
class String;

// Return an array of indices representing the lexicographical ordering of a
// set of reads, using STL's sorting algorithm.
UintArray STLStringOrder(
    const ReadSet& strings);

// Return an array of indices representing the lexicographical ordering of a
// set of reads, using radix sort.
UintArray RadixStringOrder(
    const ReadSet& strings,
    const size_t max_val);

}  // namespace overlap

#endif  // OVERLAP_SORT_H
