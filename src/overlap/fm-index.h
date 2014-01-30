#ifndef OVERLAP_FM_INDEX_H_
#define OVERLAP_FM_INDEX_H_

#include <stdint.h>
#include <sys/types.h>

#include "util.h"
#include "wavelet/wavelet-tree.h"

namespace overlap {

class String;

class FmIndex {
 public:
  FmIndex(String& bwt, size_t max_val);
  virtual ~FmIndex();

  virtual void Init() = 0;
  // Accessors.
  size_t size() const;
  size_t max_val() const;
  // Return count of values less than 'chr' in complete BWT.
  virtual uint32_t Less(uint8_t chr) const = 0;
  // Return count of values equal to 'chr' in first 'pos' values of BWT.
  virtual uint32_t Rank(uint8_t chr, uint32_t pos) const = 0;

 protected:
  const size_t size_;
  const size_t max_val_;
};

class BucketedFmIndex : public FmIndex {
 public:
  BucketedFmIndex(String& bwt, size_t max_val, size_t bucket_size);
  ~BucketedFmIndex();

  void Init();
  uint32_t Less(uint8_t chr) const;
  uint32_t Rank(uint8_t chr, uint32_t pos) const;

 private:
  const uint8_t* bwt_data_;
  // Cumulative sum of total char counts.
  uint32_t* const prefix_sum_;
  // Bucket count data.
  const size_t bucket_size_;
  const size_t num_buckets_;
  uint32_t* const buckets_;
};

class WaveletFmIndex : public FmIndex {
 public:
  WaveletFmIndex(String& bwt, size_t max_val);
  ~WaveletFmIndex();

  void Init();
  uint32_t Less(uint8_t chr) const;
  uint32_t Rank(uint8_t chr, uint32_t pos) const;

 private:
  UintArray prefix_sum_;
  const wavelet::WaveletTree wavelet_tree_;
};

}  // namespace overlap

#endif  // OVERLAP_FM_INDEX_H_
