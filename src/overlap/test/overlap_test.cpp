#include <vector>

#include <gtest/gtest.h>

#include "overlap/overlap.h"

TEST(OverlapTest, Usual) {
  overlap::Overlap o(10, 20, 50, 60, overlap::Overlap::EB, 12);
  EXPECT_EQ(10, o.read_one);
  EXPECT_EQ(20, o.read_two);
  EXPECT_EQ(50, o.len_one);
  EXPECT_EQ(60, o.len_two);
  EXPECT_EQ(overlap::Overlap::EB, o.type);
  EXPECT_EQ(12, o.score);
}

TEST(OverlapTest, Compare) {
  overlap::Overlap o(1, 2, 3, 4, overlap::Overlap::EB, 10);
  overlap::Overlap p(o);
  overlap::OverlapCmp cmp;

  EXPECT_FALSE(p < o);
  EXPECT_FALSE(cmp(&p, &o));

  ++p.score;
  EXPECT_LT(p, o);
  EXPECT_TRUE(cmp(&p, &o));

  p = o;
  --p.len_two;
  EXPECT_LT(p, o);
  EXPECT_TRUE(cmp(&p, &o));

  p = o;
  ++p.len_one;
  EXPECT_LT(o, p);
  EXPECT_TRUE(cmp(&o, &p));

  p = o;
  p.type = overlap::Overlap::BE;
  EXPECT_LT(o, p);
  EXPECT_TRUE(cmp(&o, &p));

  p = o;
  --p.read_two;
  EXPECT_LT(p, o);
  EXPECT_TRUE(cmp(&p, &o));

  p = o;
  --p.read_one;
  EXPECT_LT(p, o);
  EXPECT_TRUE(cmp(&p, &o));
}

TEST(OverlapSetTest, Usual) {
  std::vector<overlap::Overlap> raw_overlaps {
    overlap::Overlap(1, 2, 3, 4, overlap::Overlap::EB, 10),
    overlap::Overlap(1, 3, 3, 5, overlap::Overlap::BE, 12),
    overlap::Overlap(1, 2, 2, 3, overlap::Overlap::BE, 5),
    overlap::Overlap(2, 3, 2, 4, overlap::Overlap::BB, 8),
    overlap::Overlap(2, 3, 2, 3, overlap::Overlap::BB, 6),
    overlap::Overlap(3, 4, 2, 3, overlap::Overlap::EE, 8),
    overlap::Overlap(3, 5, 2, 3, overlap::Overlap::EB, 6)
  };
  overlap::OverlapSet overlaps(10);

  EXPECT_EQ(0, overlaps.size());

  for (uint32_t i = 0; i < raw_overlaps.size(); ++i) {
    overlaps.Add(new overlap::Overlap(raw_overlaps[i]));
    EXPECT_EQ(i + 1, overlaps.size());
  }

  for (uint32_t i = 0; i < overlaps.size(); ++i) {
    EXPECT_FALSE(raw_overlaps[i] < *overlaps.Get(i));
    EXPECT_FALSE(*overlaps.Get(i) < raw_overlaps[i]);
    EXPECT_FALSE(raw_overlaps[i] < *overlaps[i]);
    EXPECT_FALSE(*overlaps[i] < raw_overlaps[i]);
  }

  overlaps.Sort();
  for (uint32_t i = 0; i < overlaps.size() - 1; ++i) {
    EXPECT_LT(*overlaps[i], *overlaps[i + 1]);
  }
}
