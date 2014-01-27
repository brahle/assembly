#include "layout/union_find.h"

namespace layout {

int UnionFind::find(int x) {
  if (x == data_[x].parent_) {
    return x;
  }
  return data_[x].parent_ = find(data_[x].parent_);
}

int UnionFind::join(int x, int y) {
  x = find(x);
  y = find(y);
  if (x == y) {
    return -1;
  }
  if (data_[x].count_ < data_[y].count_) {
    data_[x].parent_ = y;
    data_[y].count_ += data_[x].count_;
    return y;
  } else {
    data_[y].parent_ = x;
    data_[x].count_ += data_[y].count_;
    return x;
  }
}

};  // namespace layout

