#ifndef OVERLAP_SORT_H_
#define OVERLAP_SORT_H_

#include "util.h"


namespace overlap {

class ReadSet;
class String;

UintArray STLStringOrder(const ReadSet& strings);
UintArray RadixStringOrder(const ReadSet& strings, const size_t max_val);

}  // namespace overlap

#endif  // OVERLAP_SORT_H
