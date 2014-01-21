#ifndef OVERLAP_DATA_H_
#define OVERLAP_DATA_H_

#include <stdint.h>
#include <sys/types.h>

#include <vector>


namespace overlap {


const uint8_t* ReverseComplement(const uint8_t* data, size_t size);

class String {
public:
  String(const uint8_t* data, size_t size);
  virtual ~String();

  const uint8_t* data() const;
  const size_t size() const;

  const uint8_t& operator[] (const uint32_t idx) const;

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
  ReadSet();
  virtual ~ReadSet();

  virtual const Read* get(uint32_t read_idx) const = 0;
  virtual const size_t size() const = 0;
};


class VectorReadSet : public ReadSet {
public:
  VectorReadSet(size_t capacity=16);
  ~VectorReadSet();

  void add(Read* read);

  const Read* get(uint32_t read_idx) const;
  const size_t size() const;

private:
  std::vector<Read*> reads_;
};


}  // namespace overlap

#endif  // OVERLAP_DATA_H_
