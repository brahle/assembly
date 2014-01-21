#include "data.h"


namespace overlap {

const uint8_t* ReverseComplement(const uint8_t* data, size_t size) {
  uint8_t* data_rc = new uint8_t[size + 1];
  data_rc[size] = '\0';
  for (uint32_t idx = 0; idx < size; ++idx) {
    data_rc[idx] = 4 - data[idx];
  }
  return data_rc;
}

String::String(const uint8_t* data, size_t size)
    : data_(data), size_(size) {}

String::~String() {
  delete[] data_;
}

const uint8_t* String::data() const {
  return data_;
}

const size_t String::size() const {
  return size_;
}

Read::Read(const uint8_t* data, size_t size)
    : String(data, size), data_rc_(ReverseComplement(data, size)) {}

Read::~Read() {
  delete[] data_rc_;
}

const uint8_t* Read::data_rc() const {
  return data_rc_;
}

ReadSet::ReadSet() {}

ReadSet::~ReadSet() {}

VectorReadSet::VectorReadSet(size_t capacity) {
  reads_.reserve(capacity);
}

VectorReadSet::~VectorReadSet() {
  for (Read* r : reads_) {
    delete r;
  }
}

void VectorReadSet::add(Read* read) {
  reads_.push_back(read);
}

const Read* VectorReadSet::get(uint32_t read_idx) const {
  return reads_[read_idx];
}

const size_t VectorReadSet::size() const {
  return reads_.size();
}

}
