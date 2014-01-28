#include "test/unitigging_test.h"
#include <overlap/read.h>
#include <overlap/overlap.h>
#include <layout/better_overlap.h>

#include <cstdio>
#include <cstring>

namespace test {

UnitiggingTest::UnitiggingTest() {
}

UnitiggingTest::~UnitiggingTest() {
}

overlap::Read* UnitiggingTest::makeRead(const char* data) {
  size_t len = strlen(data);
  uint8_t* uint_data = new uint8_t[len];
  memcpy(uint_data, data, len);
  return new overlap::Read(uint_data, len);
}

UnitiggingIsTransitiveTest::UnitiggingIsTransitiveTest() {
}

UnitiggingIsTransitiveTest::~UnitiggingIsTransitiveTest() {
}

bool UnitiggingIsTransitiveTest::run() {
  overlap::ReadSet read_set(3);
  read_set.Add(makeRead("AAAAAAAAAABBBBBBBBBBBCCCCCCC"));
  read_set.Add(makeRead("BBBBBBBBBBCCCCCCCCCCCDDDDD"));
  read_set.Add(makeRead("CCCCCCCCCCDDDDDDDDDDDAAA"));
  overlap::OverlapSet overlap_set(4);
  overlap_set.Add(new overlap::Overlap(0, 1, 17, 17, overlap::Overlap::Type::EB, 17));
  overlap_set.Add(new overlap::Overlap(1, 2, 15, 15, overlap::Overlap::Type::EB, 15));
  overlap_set.Add(new overlap::Overlap(0, 2, 7, 7, overlap::Overlap::Type::EB, 7));
  overlap_set.Add(new overlap::Overlap(2, 0, 3, 3, overlap::Overlap::Type::EB, 3));
  layout::Unitigging unitigging(&read_set, &overlap_set);
  if (!unitigging.isTransitive(
          unitigging.overlaps_[2],
          unitigging.overlaps_[0],
          unitigging.overlaps_[1])) {
    return false;
  }
  if (unitigging.isTransitive(
          unitigging.overlaps_[3],
          unitigging.overlaps_[1],
          unitigging.overlaps_[0])) {
    return false;
  }
  return true;
}

UnitiggingTestRunner::UnitiggingTestRunner() {
}

UnitiggingTestRunner::~UnitiggingTestRunner() {
  for (auto test : tests_) {
    delete test;
  }
}

void UnitiggingTestRunner::run() {
  int total = 0;
  int successful = 0;

  for (auto test : tests_) {
    total += 1;
    successful += test->run();
  }

  if (total == successful) {
    printf("\e[32mALL OK!\033[m (%d/%d)\n", total, successful);
  } else {
    printf(
        "\e[31mFAILURE: %d tests FAILED!\033[m (%d/%d = %.2lf%% OK)\n",
        total-successful,
        successful,
        total,
        ((double)successful)/total);
  }
}

void UnitiggingTestRunner::addTest(UnitiggingTest* test) {
  tests_.emplace_back(test);
}

};  // namespace test

int main() {
  int success = 0;
  int total = 0;
  test::UnitiggingTestRunner ut;
  ut.addTest(new test::UnitiggingIsTransitiveTest());
  ut.run();

  return 0;
}
