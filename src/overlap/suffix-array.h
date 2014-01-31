#ifndef OVERLAP_BWT_H_
#define OVERLAP_BWT_H_

#include <stdint.h>
#include <sys/types.h>

namespace overlap {

class ReadSet;
class String;

// Interface for suffix array construction algorithms.
class SACA {
  public:
    SACA();
    virtual ~SACA();

    // Return a new string (without owner) representing the Burrows-Wheeler
    // transformation of the set of reads, assuming values in reads are in
    // range [1, alphabet_size].
    virtual String* BuildBWT(const ReadSet& reads, size_t alphabet_size) = 0;
};

// Yuta Mori's SACA implementation.
class SaisSACA : public SACA {
  public:
    SaisSACA();

    // See comments for SACA::BuildBWT().
    String* BuildBWT(const ReadSet& reads, size_t alphabet_size);
};

}  // namespace overlap

#endif  // OVERLAP_BWT_H_
