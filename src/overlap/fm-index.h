#ifndef OVERLAP_FM_INDEX_H_
#define OVERLAP_FM_INDEX_H_

#include <stdint.h>
#include <sys/types.h>

#include "util.h"
#include "wavelet/wavelet-tree.h"

namespace overlap {

class String;

// Abstract class (interface) representing a FM-index.
class FmIndex {
 public:
  // Construct new FM-index using BWT string with values in range [0,
  // max_val].
  FmIndex(String& bwt, size_t max_val);
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
  // Length of BWT. Used as upper bound when searching.
  const size_t size_;
  // Maximal value of an element in BWT.
  const size_t max_val_;
};

// Implementation of FM index that uses cumulative bucket counts for rank.
class BucketedFmIndex : public FmIndex {
 public:
  // Construct new FM-index that caches rank for every 'bucket_size' position.
  BucketedFmIndex(String& bwt, size_t max_val, size_t bucket_size);
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
class WaveletFmIndex : public FmIndex {
 public:
  WaveletFmIndex(String& bwt, size_t max_val);
  ~WaveletFmIndex();

  // Must be called after construction.
  void Init();

  // See comments in FmIndex.
  uint32_t Less(uint8_t chr) const;
  uint32_t Rank(uint8_t chr, uint32_t pos) const;

 private:
  // Cumulative sum of total char counts.
  UintArray prefix_sum_;
  // Wavelet tree for rank queries.
  const wavelet::WaveletTree wavelet_tree_;
};

}  // namespace overlap

#endif  // OVERLAP_FM_INDEX_H_
