#ifndef OVERLAP_READ_H_
#define OVERLAP_READ_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <memory>
#include <vector>

#include "util.h"


namespace overlap {

// Basic wrapper around raw C-style strings.
class String {
public:
  // Construct a new String, taking ownership of 'data'.
  String(uint8_t* data, size_t size);
  virtual ~String();
  // Return raw string data.
  uint8_t* data() const;
  uint8_t* data();
  // Return string size.
  size_t size() const;
  // Get character at position 'pos'.
  const uint8_t& operator[](const uint32_t pos) const;
  uint8_t& operator[](const uint32_t pos);
  // Compare strings lexicographically.
  bool operator< (const String& other) const;

protected:
  // String data. Owned.
  //uint8_t* data_;
  std::unique_ptr<uint8_t[]> data_;
  // String size, excluding null character.
  const size_t size_;

  DISALLOW_COPY_AND_ASSIGN(String);
};

// Extension of String to support additional data for reads.
class Read : public String {
public:
  // Construct a new Read, taking ownership of 'data'.
  Read(uint8_t* data, size_t size, uint32_t id, uint32_t orig_id);
  ~Read();
  // Return read ID (internal).
  uint32_t id() const;
  // Return original read ID (external).
  uint32_t orig_id() const;

private:
  // 0-based index used internally.
  const uint32_t id_;
  // 0-based index of this read in the file it came from.
  const uint32_t orig_id_;

  DISALLOW_COPY_AND_ASSIGN(Read);
};

// Following methods have two forms: C-style string and Read form. Both of them
// allocate new memory that needs an owner. Read form copies read data too.
//
// Transform a string of DNA to an integer array.
// A -> 1, C -> 2, G -> 3, T -> 4
uint8_t* DNAToArray(uint8_t* data, size_t size);
Read* DNAToArray(Read& read);

// Transform an integer array of DNA to a string.
// 1 -> A, 2 -> C, 3 -> G, 4 -> T
uint8_t* ArrayToDNA(uint8_t* data, size_t size);
Read* ArrayToDNA(Read& read);

// Create a reverse complement of the integer array of DNA.
uint8_t* ReverseComplement(uint8_t* data, size_t size);
Read* ReverseComplement(Read& read);

// Print read in integer form in it's string form.
void PrintRead(FILE* fp, const Read& read);

// Container for reads.
class ReadSet {
public:
  // Create a new ReadSet given the approximate number of reads to be added.
  ReadSet(size_t capacity);
  ~ReadSet();

  void Add(Read* read);
  const Read* Get(uint32_t read_idx) const;
  Read* Get(uint32_t read_idx);

  size_t size() const;

  const Read* operator[](const uint32_t idx) const;
  Read* operator[](const uint32_t idx);

private:
  std::vector<std::unique_ptr<Read>> reads_;

  DISALLOW_COPY_AND_ASSIGN(ReadSet);
};

ReadSet* ReadFasta(FILE* fp);

}  // namespace overlap

#endif  // OVERLAP_READ_H_
