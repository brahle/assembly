#include <cassert>
#include <cstring>
#include <algorithm>
#include <memory>

#include "read.h"
#include "sais.h"
#include "suffix-array.h"

namespace overlap {

SACA::SACA() {
}

SACA::~SACA() {
}

SaisSACA::SaisSACA() {
}

String* SaisSACA::BuildBWT(const ReadSet& reads, size_t depth) {
  size_t num_strings = reads.size();
  size_t bwt_size = num_strings;

  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    assert(reads[read_idx]->size() > 0);
    bwt_size += reads[read_idx]->size();
  }

  std::unique_ptr<int[]> super_string(new int[bwt_size + 1]);
  uint32_t ss_pos = 0;
  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    const Read* read = reads[read_idx];
    super_string[ss_pos++] = read_idx;
    for (uint32_t chr_idx = 0; chr_idx < read->size(); ++chr_idx) {
      assert((*read)[chr_idx] > 0 && (*read)[chr_idx] <= depth);
      assert(ss_pos < bwt_size);
      super_string[ss_pos++] = (*read)[chr_idx] + num_strings - 1;
    }
  }
  super_string[bwt_size] = '\0';

  std::unique_ptr<int[]> bwt32(new int[bwt_size + 1]);
  std::unique_ptr<int32_t[]> A(new int32_t[bwt_size + 1]);
  bwt32[bwt_size] = '\0';

  int32_t ret = sais_int_bwt(super_string.get(), bwt32.get(), A.get(), (int)bwt_size, num_strings + depth);
  if (ret < 0) {
    fprintf(stderr, "SAIS failed (%d).\n", ret);
    return nullptr;
  }

  uint8_t* bwt = new uint8_t[bwt_size + 1];
  for (uint32_t bwt_pos = 0; bwt_pos <= bwt_size; ++bwt_pos) {
    int c = bwt32[bwt_pos];
    bwt[bwt_pos] = ((size_t)c < num_strings ? 0 : c - num_strings + 1);
  }

  return new String(bwt, bwt_size);
}


Sais2SACA::Sais2SACA() {
}

String* Sais2SACA::BuildBWT(const ReadSet& reads, size_t depth) {
  size_t num_strings = reads.size();
  size_t bwt_size = num_strings;

  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    assert(reads[read_idx]->size() > 0);
    bwt_size += reads[read_idx]->size();
  }

  std::unique_ptr<uint8_t[]> super_string(new uint8_t[bwt_size + 1]);
  uint32_t ss_pos = 0;
  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    const Read* read = reads[read_idx];
    super_string[ss_pos++] = 0;
    memcpy(super_string.get() + ss_pos, read->data(), read->size());
    ss_pos += read->size();
  }

  std::unique_ptr<uint8_t[]> bwt(new uint8_t[bwt_size + 1]);
  std::unique_ptr<int32_t[]> A(new int32_t[bwt_size + 1]);
  bwt[bwt_size] = '\0';

  int32_t ret = sais_bwt(super_string.get(), bwt.get(), A.get(), (int)bwt_size);

  if (ret < 0) {
    fprintf(stderr, "SAIS failed (%d).\n", ret);
    return nullptr;
  }

  return new String(bwt.release(), bwt_size);
}


}  // namespace overlap
