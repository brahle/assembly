#ifndef OVERLAP_DATA_H_
#define OVERLAP_DATA_H_

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <sys/types.h>

#include <vector>


namespace overlap {

enum direction_t {
  EB, BE
};

const uint8_t* ReverseComplement(const uint8_t* data, size_t size);

class Edge;

class String {
public:
  String(const uint8_t* data, size_t size);
  virtual ~String();

  const uint8_t* data() const;
  const size_t size() const;

  const uint8_t& operator[] (const uint32_t idx) const;
  const bool operator< (const String& other) const;

private:
  const uint8_t* data_;
  const size_t size_;
};

class Read : public String {
public:
  Read(const uint8_t* data, size_t size);
  ~Read();

  const uint8_t* data_rc() const;

private:
  const uint8_t* data_rc_;
};

class ReadSet {
public:
  ReadSet(size_t capacity);
  ~ReadSet();

  void add(Read* read);

  const size_t size() const;

  const Read* get(uint32_t read_idx) const;
  Read* const& operator[] (const uint32_t idx) const;

private:
  std::vector<Read*> reads_;
};

ReadSet* ReadFasta(FILE* fd);

class Edge {
  Read *A_, *B_;
  direction_t direction_;
  size_t sizeA_, sizeB_;

public:
  Edge();
  Edge(Read*, Read*, direction_t, uint32_t, uint32_t);
  ~Edge();

  const bool suf(Read*) const;
  const size_t hang(Read*) const;
  const size_t size() const;
  const size_t size(Read*) const;
};

}  // namespace overlap

#endif  // OVERLAP_DATA_H_
