#include <cassert>
#include <cstdio>
#include <cstring>

#include <gflags/gflags.h>

#include "read.h"

DEFINE_int32(min_read_size, 60, "");

namespace overlap {

String::String(uint8_t* data, size_t size)
    : data_(data), size_(size) {
}

String::~String() {
}

const uint8_t* String::data() const {
  return data_.get();
}

uint8_t* String::data() {
  return data_.get();;
}

size_t String::size() const {
  return size_;
}

const uint8_t& String::operator[] (const uint32_t idx) const {
  assert(idx <= size_);
  return data_[idx];
}

uint8_t& String::operator[] (const uint32_t idx) {
  assert(idx <= size_);
  return data_[idx];
}

bool String::operator< (const String& other) const {
  return strcmp((const char*)data_.get(), (const char*)other.data_.get()) < 0;
}

Read::Read(uint8_t* data, size_t size, uint32_t id, uint32_t orig_id)
    : String(data, size),
      id_(id),
      orig_id_(orig_id) {
}

Read::~Read() {
}

uint32_t Read::id() const {
  return id_;
}

uint32_t Read::orig_id() const {
  return orig_id_;
}

uint8_t* DNAToArray(uint8_t* data, size_t size) {
  uint8_t* array = new uint8_t[size + 1];
  array[size] = '\0';
  for (uint32_t pos = 0; pos < size; ++pos) {
    uint8_t fc = 0;
    switch (data[pos]) {
    case 'A':
      fc = 1;
      break;
    case 'C':
      fc = 2;
      break;
    case 'G':
      fc = 3;
      break;
    case 'T':
      fc = 4;
      break;
    default:
      assert(0);
      break;
    }
    array[pos] = fc;
  }
  return array;
}

Read* DNAToArray(Read& read) {
  return new Read(
      DNAToArray(read.data(), read.size()),
      read.size(),
      read.id(),
      read.orig_id());
}

uint8_t* ArrayToDNA(uint8_t* data, size_t size) {
  static const char mapping[5] = "ACGT";
  uint8_t* dna = new uint8_t[size + 1];
  dna[size] = '\0';
  for (uint8_t pos = 0; pos < size; ++pos) {
    assert(data[pos] > 0 && data[pos] < 5);
    dna[pos] = mapping[data[pos] - 1];
  }
  return dna;
}

Read* ArrayToDNA(Read& read) {
  return new Read(
      ArrayToDNA(read.data(), read.size()),
      read.size(),
      read.id(),
      read.orig_id());
}

uint8_t* ReverseComplement(uint8_t* data, size_t size) {
  uint8_t* data_rc = new uint8_t[size + 1];
  data_rc[size] = '\0';
  for (uint32_t idx = 0; idx < size; ++idx) {
    assert(data[idx] > 0 && data[idx] < 5);
    data_rc[size - idx - 1] = 5 - data[idx];
  }
  return data_rc;
}

Read* ReverseComplement(Read& read) {
  return new Read(
      ReverseComplement(read.data(), read.size()),
      read.size(),
      read.id(),
      read.orig_id());
}

void PrintRead(FILE* fp, Read& read) {
  const uint8_t* ez = ArrayToDNA(read.data(), read.size());
  fprintf(fp, "%s\n", (const char*)ez);
  delete[] ez;
}

ReadSet::ReadSet(size_t capacity) {
  reads_.reserve(capacity);
}

ReadSet::~ReadSet() {
}

void ReadSet::Add(Read* read) {
  assert(read);
  reads_.emplace_back(read);
}

const Read* ReadSet::Get(uint32_t read_idx) const {
  assert(read_idx < reads_.size());
  return reads_[read_idx].get();
}

Read* ReadSet::Get(uint32_t read_idx) {
  assert(read_idx < reads_.size());
  return reads_[read_idx].get();
}

size_t ReadSet::size() const {
  return reads_.size();
}

const Read* ReadSet::operator[] (const uint32_t idx) const {
  assert(idx < reads_.size());
  return reads_[idx].get();
}

Read* ReadSet::operator[] (const uint32_t idx) {
  assert(idx < reads_.size());
  return reads_[idx].get();
}

ReadSet* ReadFasta(FILE* fp) {
  char buff[100000];
  ReadSet* read_set = new ReadSet(1 << 16);

  uint32_t id = 0;
  for (uint32_t orig_id = 0; fgets(buff, sizeof buff, fp); ++orig_id) {
    if (!fgets(buff, sizeof buff, fp)) {
      return nullptr;
    }

    size_t size = strlen(buff) - 1;

    if ((int)size >= FLAGS_min_read_size) {
      uint8_t* read_data = DNAToArray((uint8_t*)buff, size);
      read_set->Add(new Read(read_data, size, id++, orig_id));
    }
  }

  return read_set;
}

}  // namespace overlap
