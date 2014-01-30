#include <cstdint>
#include <cstdlib>

#include <gtest/gtest.h>

#include "overlap/read.h"
#include "overlap/sort.h"
#include "overlap/util.h"


uint8_t* AllocAndInit(const char* data, size_t size);

using namespace overlap;

TEST(StringOrderTest, STL) {
  ReadSet reads(20);
  reads.Add(new Read(AllocAndInit("abbabbababba", 12), 12, 0, 0));
  reads.Add(new Read(AllocAndInit("", 0), 0, 0, 0));
  reads.Add(new Read(AllocAndInit("ab", 2), 2, 0, 0));
  reads.Add(new Read(AllocAndInit("b", 1), 1, 0, 0));
  reads.Add(new Read(AllocAndInit("aba", 3), 3, 0, 0));
  reads.Add(new Read(AllocAndInit("aababab", 7), 7, 0, 0));
  reads.Add(new Read(AllocAndInit("a", 1), 1, 0, 0));
  reads.Add(new Read(AllocAndInit("abb", 3), 3, 0, 0));
  reads.Add(new Read(AllocAndInit("aa", 2), 2, 0, 0));
  reads.Add(new Read(AllocAndInit("abaabaaaa", 9), 9, 0, 0));

  UintArray order = STLStringOrder(reads);
  for (uint32_t i = 0; i < reads.size() - 1; ++i) {
    EXPECT_LT(*reads[order[i]], *reads[order[i + 1]]);
  }
}

TEST(StringOrderTest, Radix) {
  ReadSet reads(20);
  reads.Add(new Read(DNAToArray((uint8_t*)"ACCACCACACCA", 12), 12, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"", 0), 0, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"AC", 2), 2, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"C", 1), 1, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"ACA", 3), 3, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"AACACAC", 7), 7, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"A", 1), 1, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"ACC", 3), 3, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"AG", 2), 2, 0, 0));
  reads.Add(new Read(DNAToArray((uint8_t*)"ACAACAAAA", 9), 9, 0, 0));

  UintArray order = RadixStringOrder(reads, 4);
  for (uint32_t i = 0; i < reads.size() - 1; ++i) {
    EXPECT_LT(*reads[order[i]], *reads[order[i + 1]]);
  }
}
