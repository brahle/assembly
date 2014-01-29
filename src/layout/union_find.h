#ifndef LAYOUT_UNION_FIND_H_
#define LAYOUT_UNION_FIND_H_

namespace layout {

class UnionFind {
 public:
  explicit UnionFind(int n);
  ~UnionFind();

  int find(int x);
  int join(int x, int y);
 private:
  struct Node {
    int value_;
    int count_;
    int parent_;
    Node() : Node(0, 1, -1) {}
    Node(int value, int parent) : Node(value, 1, parent) {}
    Node(int value, int count, int parent) :
      value_(value), count_(count), parent_(parent) {}
  };

  Node *data_;
};

}  // namespace layout

#endif  // LAYOUT_UNION_FIND_H_

