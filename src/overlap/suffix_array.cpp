#include <stddef.h>

#include <sais.h>

#include "read.h"
#include "suffix_array.h"


namespace overlap {


SACA::SACA() {}

SACA::~SACA() {}

SaisSACA::SaisSACA() {}

String* SaisSACA::BuildBWT(const ReadSet& reads, size_t depth) {
  size_t num_strings = reads.size();
  size_t bwt_size = num_strings;

  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    bwt_size += reads[read_idx]->size();
  }

  sa_int32_t* super_string = new sa_int32_t[bwt_size + 1];
  uint32_t ss_pos = 0;
  for (uint32_t read_idx = 0; read_idx < num_strings; ++read_idx) {
    const Read* read = reads[read_idx];
    super_string[ss_pos++] = read_idx;
    for (uint32_t chr_idx = 0; chr_idx < read->size(); ++chr_idx) {
      super_string[ss_pos++] = (*read)[chr_idx] + num_strings - 1;
    }
  }
  super_string[bwt_size] = '\0';

  sa_int32_t* bwt32 = new sa_int32_t[bwt_size];

  if (sais_i32_bwt(super_string, bwt32, NULL, (sa_int32_t)bwt_size, depth) < 0) {
    return NULL;
  }

  uint8_t* bwt = new uint8_t[bwt_size + 1];
  for (uint32_t bwt_pos = 0; bwt_pos <= bwt_size; ++bwt_pos) {
    sa_int32_t c = bwt32[bwt_pos];
    bwt[bwt_pos] = ((size_t)c < num_strings ? 0 : c - num_strings + 1);
  }

  delete[] super_string;
  return new String(bwt, bwt_size);
}


}  // namespace overlap
