#ifndef LAYOUT_LAYOUT_UTILS_H_
#define LAYOUT_LAYOUT_UTILS_H_

#include <algorithm>

#include <overlap/read.h>
#include <overlap/overlap.h>
#include <layout/contig.h>

namespace layout {

overlap::Read* ReadOneReadAfg(FILE *fd);
overlap::ReadSet* ReadReadsAfg(FILE *fd);
overlap::OverlapSet* ReadOverlapsAfg(overlap::ReadSet* read_set, FILE *fd);

int n50(ContigSet* contig_set);
};  // namespace layout

#endif  // LAYOUT_LAYOUT_UTILS_H_
