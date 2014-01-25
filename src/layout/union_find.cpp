#ifndef OVERLAP_UNIONFIND_CPP_
#define OVERLAP_UNIONFIND_CPP_

#include "union_find.hpp"

namespace layout {

int UnionFind::find(int x) {
  if (x == data_[x].parent_) {
    return x;
  }
  return data_[x].parent_ = find(data_[x].parent_);
}

bool UnionFind::join(int x,int y) {
  x = find(x);
  y = find(y);
  if (x == y) {
    return false;
  }
  if (data_[x].count_ < data_[y].count_) {
    data_[x].parent_ = y;
    data_[y].count_ += data_[x].count_;
  } else {
    data_[y].parent_ = x;
    data_[x].count_ += data_[y].count_;
  }
  return true;
}

};

#endif
