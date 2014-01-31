#ifndef LAYOUT_LAYOUT_UTILS_H_
#define LAYOUT_LAYOUT_UTILS_H_

#include <overlap/read.h>
#include <overlap/overlap.h>

namespace layout {

overlap::Read* ReadOneReadAfg(FILE *fd);
overlap::ReadSet* ReadReadsAfg(FILE *fd);
overlap::OverlapSet* ReadOverlapsAfg(overlap::ReadSet* read_set, FILE *fd);

};  // namespace layout

#endif  // LAYOUT_LAYOUT_UTILS_H_
