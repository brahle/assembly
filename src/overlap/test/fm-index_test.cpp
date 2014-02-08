#include <cstdint>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "overlap/fm-index.h"
#include "overlap/read.h"
#include "overlap/sort.h"
#include "overlap/suffix-array.h"
#include "overlap/util.h"


TEST(BucketedFmIndexTest, Rank) {
  size_t size = 100000;
  uint8_t* bwt_data = new uint8_t[size + 1];
  std::unique_ptr<uint8_t[]> bwt_data_cpy(new uint8_t[size + 1]);
  uint32_t depth = 5;

  srand(time(0));
  for (uint32_t i = 0; i < size; ++i) {
    bwt_data[i] = rand() % depth;
    bwt_data_cpy[i] = bwt_data[i];
  }
  bwt_data_cpy[size] = '\0';
  bwt_data[size] = '\0';

  overlap::String* bwt = new overlap::String(bwt_data, size);
  //overlap::BucketedFmIndex fmi(bwt, 4, 16);
  //overlap::WaveletFmIndex fmi(bwt, depth - 1);
  overlap::BitBucketFmIndex fmi(bwt, 4);

  fmi.Init();

  ASSERT_EQ(size, fmi.size());

  uint32_t cnt[5] = {0};
  for (uint32_t i = 1; i <= size; ++i) {
    if (i) {
      cnt[bwt_data_cpy[i - 1]]++;
    }
    for (uint8_t c = 0; c < depth; ++c) {
      //ASSERT_GE(size, fmi.Rank(c, i));
      EXPECT_EQ(cnt[c],  fmi.Rank(c, i));
    }
  }

  uint32_t csum = 0;
  for (uint8_t c = 0; c < depth; ++c) {
    EXPECT_EQ(csum, fmi.Less(c));
    csum += cnt[c];
  }
  ASSERT_EQ(csum, fmi.size());
}

TEST(BucketedFmIndexTest, Integration) {
  std::vector<std::string> raw_reads {
    "ACGTATACGTTAGCTATTCG",
    "ACTAGGCTATGTCATTAACG",
    "TTAGCTAGCTAGGCTAGCTA",
    "ATCGAGGCTAGCTACGCGCT",
    "ATCGATCGATTTAGCGGGAT",
    "GTACGATCGATCGA",
    "AGTCGATACGTA",
    "TTCGAGTCAGATTA",
    "ACGTTTGTGTA",
    "GGGATGTCGATGTA",
    "TTACGTAGTATC",
    "ATCGGGA"
  };

  overlap::ReadSet reads(20);
  for (uint32_t i = 0; i < raw_reads.size(); ++i) {
    uint8_t* read = overlap::DNAToArray(
        (uint8_t*)raw_reads[i].c_str(), raw_reads[i].size());
    reads.Add(new overlap::Read(read, raw_reads[i].size(), i, i));
  }

  overlap::UintArray order = overlap::STLStringOrder(reads);
  overlap::Sais2SACA saca;
  std::unique_ptr<overlap::String> bwt(saca.BuildBWT(reads, 4));
  overlap::BucketedFmIndex fmi(bwt.release(), 4, 32);
  fmi.Init();

  ASSERT_EQ(reads.size(), fmi.Less(1));

  size_t cnt = 0;
  for (uint32_t i = 0; i < reads.size(); ++i) {
    overlap::Read* read = reads[i];

    uint32_t low = 0;
    uint32_t high = fmi.size();
    for (uint32_t j = 1; j <= read->size(); ++j) {
      std::set<uint32_t> res;

      for (uint32_t k = 0; k < reads.size(); ++k) {
        overlap::Read* read2 = reads[k];
        if (read2->size() < j) continue;

        if (!strncmp((const char*)read->data() + read->size() - j, (const char*)read2->data(), j)) {
          res.insert(k);
        }
      }

      uint8_t p = (*read)[read->size() - j];
      low = fmi.Less(p) + fmi.Rank(p, low);
      high = fmi.Less(p) + fmi.Rank(p, high);

      uint8_t flow = fmi.Rank(0, low);
      uint8_t fhigh = fmi.Rank(0, high);
      ASSERT_EQ(res.size(), fhigh - flow);
      cnt += res.size();

      while (flow < fhigh) {
        EXPECT_NE(res.end(), res.find(order[flow++]));
      }
    }
  }

  ASSERT_NE(0, cnt);
}
