#include <cassert>
#include <cstring>

#include "read.h"
#include "fm-index.h"
#include "wavelet/wavelet-tree.h"

namespace overlap {

FmIndex::FmIndex(
    String* bwt,
    size_t max_val)
    : bwt_(bwt),
      size_(bwt->size()),
      max_val_(max_val) {
}

FmIndex::~FmIndex() {
}

size_t FmIndex::size() const {
  return size_;
}

size_t FmIndex::max_val() const {
  return max_val_;
}

BucketedFmIndex::BucketedFmIndex(
    String* bwt,
    size_t max_val,
    size_t bucket_size)
    : FmIndex(bwt, max_val),
      bwt_data_(bwt->data()),
      prefix_sum_(new uint32_t[max_val_ + 2]),
      bucket_size_(bucket_size),
      num_buckets_((size_ - 1) / bucket_size_ + 2),
      buckets_(new uint32_t[num_buckets_ * max_val_]) {
  Init();
}

BucketedFmIndex::~BucketedFmIndex() {
  delete [] prefix_sum_;
  delete [] buckets_;
}

void BucketedFmIndex::Init() {
  memset(buckets_, 0, num_buckets_ * max_val_ * sizeof(uint32_t));

  for (uint32_t bidx = 1; bidx < num_buckets_; ++bidx) {
    uint32_t* curr_bucket = buckets_ + bidx * max_val_;
    uint32_t* prev_bucket = buckets_ + (bidx - 1) * max_val_;

    for (uint32_t cidx = 0; cidx < max_val_; ++cidx) {
      curr_bucket[cidx] = prev_bucket[cidx];
    }
    const uint32_t start_idx = (bidx - 1) * bucket_size_;
    for (uint32_t pos = 0; pos < bucket_size_ && start_idx + pos < size_; ++pos) {
      uint8_t chr = bwt_data_[start_idx + pos];
      if (chr > 0) {
        assert(chr <= max_val_);
        ++curr_bucket[chr - 1];
      }
    }
  }

  memset(prefix_sum_, 0, (max_val_ + 2) * sizeof(uint32_t));
  for (uint32_t char_idx = 1; char_idx <= max_val_ + 1; ++char_idx) {
    prefix_sum_[char_idx] = prefix_sum_[char_idx - 1] + Rank(char_idx - 1, size_);
    assert(prefix_sum_[char_idx] <= size_);
  }
}

uint32_t BucketedFmIndex::Less(uint8_t chr) const {
  assert(chr <= max_val_);
  return prefix_sum_[chr];
}

uint32_t BucketedFmIndex::Rank(uint8_t chr, uint32_t pos) const {
  assert(chr <= max_val_);
  assert(pos <= size_);
  uint32_t count = 0;
  for (uint32_t idx = pos % bucket_size_; idx > 0; --idx) {
    count += (bwt_data_[pos - idx] == chr ? 1 : 0);
  }
  uint32_t* bucket = buckets_ + (pos / bucket_size_ * max_val_);
  if (chr > 0) {
    count += bucket[chr - 1];
  } else if (chr == 0) {
    count += (pos / bucket_size_) * bucket_size_;
    for (uint32_t cidx = 0; cidx < max_val_; ++cidx) {
      count -= bucket[cidx];
    }
  }
  return count;
}

WaveletFmIndex::WaveletFmIndex(String* bwt, size_t max_val)
    : bwt_(bwt),
      size_(bwt_->size()),
      max_val_(max_val),
      prefix_sum_(max_val + 1),
      wavelet_tree_((char*)bwt_->data(), size_, max_val + 1) {
}

WaveletFmIndex::~WaveletFmIndex() {
}

void WaveletFmIndex::Init() {
  prefix_sum_[0] = 0;
  for (uint32_t char_idx = 1; char_idx <= max_val_; ++char_idx) {
    prefix_sum_[char_idx] = prefix_sum_[char_idx - 1] +
                            wavelet_tree_.get_rank(char_idx - 1, size_);
  }
}

/*
uint32_t WaveletFmIndex::Less(uint8_t chr) const {
  assert(chr <= max_val_);
  return prefix_sum_[chr];
}

uint32_t WaveletFmIndex::Rank(uint8_t chr, uint32_t pos) const {
  assert(chr <= max_val_);
  assert(pos <= size_);
  return wavelet_tree_.get_rank(chr, pos);
}
*/

}  // namespace overlap
