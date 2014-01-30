#ifndef OVERLAP_WAVELET_WAVELET_TREE_H_
#define OVERLAP_WAVELET_WAVELET_TREE_H_

#include <vector>

#include "ranked-bitmap.h"

namespace wavelet {

typedef unsigned long long ullint;

class RankedBitmap;

class WaveletTree {
public:
  WaveletTree();
  WaveletTree(char* data, ullint data_sz, int alphabet_sz);

  // Expects data over an alphabet consisting of [0, 1, 2, ..., alphabet_sz-1]
  // Warning: data will become garbled
  void init(char* data, ullint data_sz, int alphabet_sz);

  // Number of occurences of c in data positions [0, 1, 2, ..., pos-1]
  ullint get_rank(char c, ullint pos) const;

  // The character at position pos (+ gets the rank of it)
  char get_char_at(ullint pos, ullint& rank) const;

  void serialize(FILE* out) const;
  void deserialize(FILE* in);

private:
  struct Node {
    RankedBitmap bitmap;
  };

  std::vector<Node> nodes;
  int alphabet_sz;

  void build_tree(int x, int a_lo, int a_hi, char* data, ullint data_sz);
};

}  // namespace wavelet

#endif  // OVERLAP_WAVELET_WAVELET_TREE_H_
