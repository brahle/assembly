#ifndef OVERLAP_FM_INDEX_H_
#define OVERLAP_FM_INDEX_H_

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <memory>

#include "util.h"
#include "wavelet/wavelet-tree.h"

namespace overlap {

class String;

// Abstract class (interface) representing a FM-index.
class FmIndex {
 public:
  // Construct new FM-index using BWT string with values in range [0,
  // max_val].
  FmIndex(String* bwt, size_t max_val);
  virtual ~FmIndex();

  // Must be called after construction.
  virtual void Init() = 0;

  // Accessors.
  size_t size() const;
  size_t max_val() const;

  // Return count of values less than 'chr' in complete BWT.
  virtual uint32_t Less(uint8_t chr) const = 0;
  // Return count of values equal to 'chr' in first 'pos' values of BWT.
  virtual uint32_t Rank(uint8_t chr, uint32_t pos) const = 0;

 protected:
  std::unique_ptr<String> bwt_;
  // Length of BWT. Used as upper bound when searching.
  const size_t size_;
  // Maximal value of an element in BWT.
  const size_t max_val_;
};

// Implementation of FM index that uses cumulative bucket counts for rank.
class BucketedFmIndex : public FmIndex {
 public:
  // Construct new FM-index that caches rank for every 'bucket_size' position.
  BucketedFmIndex(String* bwt, size_t max_val, size_t bucket_size);
  ~BucketedFmIndex();

  // Must be called after construction.
  void Init();

  // See comments in FmIndex.
  uint32_t Less(uint8_t chr) const;
  uint32_t Rank(uint8_t chr, uint32_t pos) const;

 private:
  // BWT string. Not owned.
  const uint8_t* bwt_data_;
  // Cumulative sum of total char counts.
  uint32_t* const prefix_sum_;
  // Bucket count data (cumulative counts of chars).
  const size_t bucket_size_;
  const size_t num_buckets_;
  uint32_t* const buckets_;
};

// Implementation of FM index that uses wavelet tree for rank queries.
class WaveletFmIndex {
 public:
  WaveletFmIndex(String* bwt, size_t max_val);
  ~WaveletFmIndex();

  // Must be called after construction.
  void Init();

  // See comments in FmIndex.
  uint32_t Less(uint8_t chr) const {
    assert(chr <= max_val_);
    return prefix_sum_[chr];
  }

  uint32_t Rank(uint8_t chr, uint32_t pos) const {
    assert(chr <= max_val_);
    assert(pos <= size_);
    return wavelet_tree_.get_rank(chr, pos);
  }

  size_t size() const { return size_; }
  size_t max_val() const { return max_val_; }

 private:
  std::unique_ptr<String> bwt_;
  const size_t size_;
  const size_t max_val_;
  // Cumulative sum of total char counts.
  UintArray prefix_sum_;
  // Wavelet tree for rank queries.
  const wavelet::WaveletTree wavelet_tree_;
};

class BitBucketFmIndex {
 public:
  BitBucketFmIndex(String* bwt, size_t max_val);
  ~BitBucketFmIndex();

  void Init() {}

  uint32_t Less(uint8_t chr) const {
    return prefix_sum_[chr];
  }

  uint32_t Rank(uint8_t chr, uint32_t pos) const {
    register uint32_t div = (pos >> 6) * depth_ + chr;
    register uint32_t mod = pos & 63;
    register uint64_t mask = ((1ULL << mod) - 1) & bit_bwt_[div];
    return bucket_sums_[div] +
      bit_counts_[mask & 65535] +
      bit_counts_[(mask >> 16ULL) & 65535] +
      bit_counts_[(mask >> 32ULL) & 65535] +
      bit_counts_[(mask >> 48ULL) & 65535];
  }

  size_t size() const { return size_; }
  size_t max_val() const { return depth_ - 1; }

 private:
  const size_t size_;
  const size_t depth_;
  uint32_t* prefix_sum_;
  const size_t num_buckets_;
  uint64_t* bit_bwt_;
  uint32_t* bucket_sums_;
  uint8_t* bit_counts_;
};

}  // namespace overlap

#endif  // OVERLAP_FM_INDEX_H_
