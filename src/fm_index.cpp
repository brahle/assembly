#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include <sais.h>

using namespace std;

class FMIndex {
 public:
  FMIndex(const uint8_t* bwt, size_t size, size_t depth, size_t bucket_size) :
      bwt_(bwt), size_(size), depth_(depth), bucket_size_(bucket_size),
      num_buckets_(size_ / bucket_size_ + 1) {
    Init();
  }

  uint32_t Rank(uint8_t chr, uint32_t pos) {
    uint32_t count = 0;
    for (uint32_t idx = pos % bucket_size_; idx > 0; --idx) {
      count += (bwt_[pos - idx] == chr ? 1 : 0);
    }
    uint32_t* bucket = count_buckets_ + (pos / bucket_size_ * depth_);
    if (chr > 0) {
      count += bucket[chr - 1];
    } else if (chr == 0) {
      count += (pos / bucket_size_) * bucket_size_;
      for (uint32_t cidx = 0; cidx < depth_; ++cidx) {
        count -= bucket[cidx];
      }
    }
    return count;
  }

  ~FMIndex() {
    delete [] count_buckets_;
  }

 private:
  void Init() {
    count_buckets_ = new uint32_t[num_buckets_ * depth_];
    memset(count_buckets_, 0, num_buckets_ * depth_ * sizeof(uint32_t));

    for (uint32_t bidx = 1; bidx < num_buckets_; ++bidx) {
      uint32_t* curr_bucket = count_buckets_ + bidx * depth_;
      uint32_t* prev_bucket = count_buckets_ + (bidx - 1) * depth_;

      for (uint32_t cidx = 0; cidx < depth_; ++cidx) {
        curr_bucket[cidx] = prev_bucket[cidx];
      }
      for (uint32_t pos = 0; pos < bucket_size_; ++pos) {
        uint8_t chr = bwt_[(bidx - 1) * bucket_size_ + pos];
        if (chr > 0) {
          ++curr_bucket[chr - 1];
        }
      }
    }
  }

  const uint8_t* bwt_;
  const size_t size_;
  const size_t depth_;
  const size_t bucket_size_;
  const size_t num_buckets_;
  uint32_t* count_buckets_;
};
