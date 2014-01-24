#include <cassert>
#include <cstdio>
#include <cstring>

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

const uint8_t& String::operator[] (const uint32_t idx) const {
  return data_[idx];
}

const bool String::operator< (const String& other) const {
  return strcmp((const char*)data_, (const char*)other.data_) < 0;
}

Read::Read(const uint8_t* data, size_t size)
    : String(data, size), data_rc_(ReverseComplement(data, size)) {}

Read::~Read() {
  delete[] data_rc_;
}

const uint8_t* Read::data_rc() const {
  return data_rc_;
}

ReadSet::ReadSet(size_t capacity) {
  reads_.reserve(capacity);
}

ReadSet::~ReadSet() {
  for (Read* r : reads_) {
    delete r;
  }
}

void ReadSet::add(Read* read) {
  reads_.push_back(read);
}

const size_t ReadSet::size() const {
  return reads_.size();
}

const Read* ReadSet::get(uint32_t read_idx) const {
  return reads_[read_idx];
}

Read* const& ReadSet::operator[] (const uint32_t idx) const {
  return reads_[idx];
}

ReadSet* ReadFasta(FILE* fd) {
  char buff[100000];
  ReadSet* read_set = new ReadSet(1 << 16);

  while (fgets(buff, sizeof buff, fd)) {
    assert(fgets(buff, sizeof buff, fd));
    size_t size = strlen(buff);
    uint8_t* read_data = new uint8_t[size];
    buff[size - 1] = '\0';
    strcpy((char*)read_data, buff);
    read_set->add(new Read(read_data, size));
  }

  return read_set;
}


}  // namespace overlap
