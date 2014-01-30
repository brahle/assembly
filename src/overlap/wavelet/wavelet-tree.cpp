#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ranked-bitmap.h"
#include "wavelet-tree.h"


namespace wavelet {

using namespace std;

WaveletTree::WaveletTree() {
}

WaveletTree::WaveletTree(char* data, ullint data_sz, int alphabet_sz) {
  init(data, data_sz, alphabet_sz);
}

// Expects data over an alphabet consisting of [0, 1, 2, ..., alphabet_sz-1]
// Warning: data will become garbled
void WaveletTree::init(char* data, ullint data_sz, int alphabet_sz) {
  this->alphabet_sz = alphabet_sz;

  int pow2 = 1;
  while (pow2 < alphabet_sz) pow2 *= 2;
  nodes.resize(2*pow2);

  build_tree(1, 0, alphabet_sz, data, data_sz);
}

void WaveletTree::build_tree(int x, int a_lo, int a_hi, char* data, ullint data_sz) {
  if (a_lo+1 == a_hi) {
    return; // done
  }

  int a_mid = (a_lo + a_hi) / 2;

  // create bitmask
  nodes[x].bitmap.init(data, data_sz, data_sz, a_lo, a_mid, a_hi);

  // partition
  ullint ptr = std::stable_partition(data, data+data_sz, [a_mid](char a) {
      return a < a_mid;
    }) - data;

  build_tree(2*x, a_lo, a_mid, data, ptr);
  build_tree(2*x+1, a_mid, a_hi, data+ptr, data_sz-ptr);
}

ullint WaveletTree::get_rank(char c, ullint pos) const {
  assert(0 <= c && c < alphabet_sz);

  int tree_ptr = 1;
  int lo = 0, hi = alphabet_sz;

  while (lo+1 < hi) {
    int mid = (lo + hi) / 2;
    bool bit = (lo <= c && c < mid) ? 0 : 1;
    pos = nodes[tree_ptr].bitmap.get_rank(bit, pos);

    if (bit == 0) {
      tree_ptr = 2*tree_ptr;
      hi = mid;
    } else {
      tree_ptr = 2*tree_ptr + 1;
      lo = mid;
    }
  }

  return pos;
}

char WaveletTree::get_char_at(ullint pos, ullint& rank) const {
  int tree_ptr = 1;
  int lo = 0, hi = alphabet_sz;

  while (lo+1 < hi) {
    int mid = (lo + hi) / 2;

    bool bit = nodes[tree_ptr].bitmap.get_bit(pos);
    ullint bit_rank = nodes[tree_ptr].bitmap.get_rank(bit, pos); // TODO: optimize?

    pos = bit_rank;

    if (bit == 0) {
      tree_ptr = 2*tree_ptr;
      hi = mid;
    } else {
      tree_ptr = 2*tree_ptr + 1;
      lo = mid;
    }
  }

  rank = pos;
  return lo;
}

}  // namespace wavelet
