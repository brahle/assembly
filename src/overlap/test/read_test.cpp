#include <cstdio>

#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "overlap/read.h"


uint8_t* AllocAndInit(const char* data, size_t size) {
  uint8_t* array = new uint8_t[size + 1];
  strcpy((char*)array, data);
  return array;
}

TEST(ReadTest, StringNormal) {
  uint8_t* data = AllocAndInit("wordisborn", 10);
  overlap::String tmp(data, 10);
  EXPECT_EQ(10, tmp.size());
  EXPECT_STREQ("wordisborn", (const char*)tmp.data());
  for (uint32_t idx = 0; idx < 10; ++idx) {
    EXPECT_EQ(data[idx], tmp[idx]);
  }
  EXPECT_EQ('\0', data[10]);
  EXPECT_FALSE(tmp < tmp);
}

TEST(ReadTest, StringEmpty) {
  uint8_t* data = AllocAndInit("", 0);
  overlap::String tmp(data, 0);
  EXPECT_EQ(0, tmp.size());
  EXPECT_STREQ("", (const char*)tmp.data());
  EXPECT_EQ('\0', tmp[0]);
  EXPECT_FALSE(tmp < tmp);
}

TEST(ReadTest, StringCompare) {
  std::vector<std::string> strings {
    "", "A", "C", "G", "T", "AC", "AG", "ACGA", "ATAC", "CTAG", "CG", "CTA",
    "CGGTA", "GAGTG", "GATT", "GCCA", "GCTC", "TAT", "TACGT", "TACGTGTG", "ACGTAG"
  };

  std::vector<overlap::String*> my_strings;

  for (auto s : strings) {
    my_strings.push_back(
        new overlap::String(AllocAndInit(s.c_str(), s.size()), s.size()));
  }

  for (int i = 0; i < (int)strings.size(); ++i) {
    for (int j = i + 1; j < (int)strings.size(); ++j) {
      EXPECT_EQ(strings[i] < strings[j], *my_strings[i] < *my_strings[j]);
    }
  }

  for (auto s : my_strings) {
    delete s;
  }
}

TEST(ReadTest, ReadNormal) {
  uint8_t* data = AllocAndInit("wordwasborn", 11);
  overlap::Read tmp(data, 11, 10, 15);
  EXPECT_EQ(11, tmp.size());
  EXPECT_STREQ("wordwasborn", (const char*)tmp.data());
  for (uint32_t idx = 0; idx < 11; ++idx) {
    EXPECT_EQ(data[idx], tmp[idx]);
  }
  EXPECT_EQ('\0', data[11]);
  EXPECT_FALSE(tmp < tmp);
  EXPECT_EQ(10, tmp.id());
  EXPECT_EQ(15, tmp.orig_id());
}

TEST(ReadTest, DNAToArrayChars) {
  const uint8_t* array = overlap::DNAToArray((const uint8_t*)"ACGTCGCATACGTCA", 15);
  uint8_t target[16] = {1, 2, 3, 4, 2, 3, 2, 1, 4, 1, 2, 3, 4, 2, 1, 0};
  EXPECT_STREQ((const char*)target, (const char*)array);
  delete[] array;
}

TEST(ReadTest, DNAToArrayRead) {
  overlap::Read read(AllocAndInit("ACAGACATACATGGCCT", 17), 17, 0, 10);
  uint8_t target[18] = {1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 4, 3, 3, 2, 2, 4, 0};
  std::unique_ptr<overlap::Read> new_read(overlap::DNAToArray(read));
  EXPECT_STREQ((const char*)target, (const char*)new_read->data());
  EXPECT_EQ(read.size(), new_read->size());
  EXPECT_EQ(read.id(), new_read->id());
  EXPECT_EQ(read.orig_id(), new_read->orig_id());
}

TEST(ReadTest, ArrayToDNAChars) {
  uint8_t array[16] = {1, 2, 3, 4, 2, 3, 2, 1, 4, 1, 2, 3, 4, 2, 1, 0};
  const uint8_t* dna = overlap::ArrayToDNA(array, 15);
  EXPECT_STREQ("ACGTCGCATACGTCA", (const char*)dna);
  delete[] dna;
}

TEST(ReadTest, ArrayToDNARead) {
  uint8_t array[18] = {1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 4, 3, 3, 2, 2, 4, 0};
  overlap::Read read(AllocAndInit((const char*)array, 17), 17, 0, 10);
  std::unique_ptr<overlap::Read> new_read(overlap::ArrayToDNA(read));
  EXPECT_STREQ("ACAGACATACATGGCCT", (const char*)new_read->data());
  EXPECT_EQ(read.size(), new_read->size());
  EXPECT_EQ(read.id(), new_read->id());
  EXPECT_EQ(read.orig_id(), new_read->orig_id());

}

TEST(ReadTest, ReverseComplementChars) {
  const uint8_t* fwd = overlap::DNAToArray((const uint8_t*)"ACGTCGTAGCTAG", 13);
  const uint8_t* bwd = overlap::ReverseComplement(fwd, 13);
  const uint8_t* bwd_dna = overlap::ArrayToDNA(bwd, 13);

  EXPECT_STREQ("CTAGCTACGACGT", (const char*)bwd_dna);

  delete[] fwd;
  delete[] bwd;
  delete[] bwd_dna;
}

TEST(ReadTest, ReverseComplementRead) {
  overlap::Read fwd(overlap::DNAToArray((const uint8_t*)"ACGTATCGATCGATGG", 16), 16, 3, 15);
  std::unique_ptr<overlap::Read> bwd(overlap::ReverseComplement(fwd));
  std::unique_ptr<overlap::Read> bwd_dna(overlap::ArrayToDNA(*bwd));

  EXPECT_STREQ("CCATCGATCGATACGT", (const char*)bwd_dna->data());
  EXPECT_EQ(fwd.size(), bwd_dna->size());
  EXPECT_EQ(fwd.id(), bwd_dna->id());
  EXPECT_EQ(fwd.orig_id(), bwd_dna->orig_id());
}

TEST(ReadTest, ReadSet) {
  overlap::ReadSet reads(512);
  EXPECT_EQ(0, reads.size());

  std::vector<overlap::Read*> reads_v {
    new overlap::Read(AllocAndInit("", 0), 0, 0, 0),
    new overlap::Read(AllocAndInit("a", 1), 1, 1, 1),
    new overlap::Read(AllocAndInit("ab", 2), 2, 2, 2),
    new overlap::Read(AllocAndInit("acasdb", 6), 6, 2, 2),
    new overlap::Read(AllocAndInit("asadjoaib", 9), 9, 2, 2)
  };

  for (uint32_t i = 0; i < reads_v.size(); ++i) {
    reads.Add(reads_v[i]);
    EXPECT_EQ(i + 1, reads.size());
    EXPECT_EQ(reads_v[i], reads[i]);
    EXPECT_EQ(reads_v[i], reads.Get(i));
  }
}

TEST(ReadTest, ReadFasta) {
  FILE *fp = tmpfile();
  ASSERT_NE(nullptr, fp);

  std::vector<std::string> strings {
    "A", "C", "G", "T", "AC", "AG", "ACGA", "ATAC", "CTAG", "CG", "CTA",
    "CGGTA", "GAGTG", "GATT", "GCCA", "GCTC", "TAT", "TACGT", "TACGTGTG", "ACGTAG"
  };

  for (uint32_t i = 0; i < strings.size(); ++i) {
    fprintf(fp, ">%d\n", i);
    fprintf(fp, "%s\n", strings[i].c_str());
  }

  rewind(fp);
  std::unique_ptr<overlap::ReadSet> reads(overlap::ReadFasta(fp, 1));
  ASSERT_EQ(strings.size(), reads->size());
  for (uint32_t i = 0; i < strings.size(); ++i) {
    std::unique_ptr<overlap::Read> read(overlap::ArrayToDNA(*reads->Get(i)));
    ASSERT_STREQ(strings[i].c_str(), (const char*)read->data());
  }

  std::vector<std::string> strings5 {
    "CGGTA", "GAGTG", "TACGT", "TACGTGTG", "ACGTAG"
  };

  rewind(fp);
  reads.reset(overlap::ReadFasta(fp, 5));
  ASSERT_EQ(strings5.size(), reads->size());
  for (uint32_t i = 0; i < strings5.size(); ++i) {
    std::unique_ptr<overlap::Read> read(overlap::ArrayToDNA(*reads->Get(i)));
    ASSERT_STREQ(strings5[i].c_str(), (const char*)read->data());
  }

  fclose(fp);
}
