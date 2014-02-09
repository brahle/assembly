#include <cassert>
#include <cstdint>
#include <algorithm>
#include <map>
#include <memory>

#include <gflags/gflags.h>

#include "myers.h"
#include "overlap.h"
#include "read.h"
#include "validate.h"

DEFINE_double(error_rate, 0.02, "");
DEFINE_int32(error_mult, 3, "");

namespace overlap {

namespace {

uint32_t OverlapConfidence(Overlap* o) {
  return o->len_one + o->len_two - o->score;
}

}  // unnamed namespace

OverlapSet* ValidateCandidates(
    const ReadSet& reads,
    const OverlapSet& candidates) {

  typedef std::pair<uint32_t, uint32_t> Pair;
  std::map<Pair, Overlap*> best;

  std::unique_ptr<OverlapSet> overlaps(new OverlapSet(candidates.size()));

  const double extra_ratio = 1 + (FLAGS_error_rate * FLAGS_error_mult);

  for (uint32_t oid = 0; oid < candidates.size(); ++oid) {
    const Overlap* o = candidates[oid];

    const Read* read_one = reads[o->read_one];
    const Read* read_two = reads[o->read_two];

    int len_two, score;
    int ret = MyersEditDistance(
        read_one->data() + read_one->size() - o->len_one,
        o->len_one,
        read_two->data(),
        std::min((int)(o->len_one * extra_ratio), (int)read_two->size()),
        5,
        (int)(o->len_one * FLAGS_error_rate * FLAGS_error_mult),
        MYERS_MODE_SHW,
        &score,
        &len_two);

    if (ret == MYERS_STATUS_OK and len_two != -1 and score != -1) {
      Overlap* new_o = new Overlap(*o);
      new_o->len_two = len_two + 1;
      new_o->score = score;
      new_o->score = OverlapConfidence(new_o);

      auto elem = best.find(std::make_pair(new_o->read_one, new_o->read_two));
      if (elem == best.end() or elem->second->score < new_o->score) {
        best[std::make_pair(o->read_one, o->read_two)] = new_o;
        best[std::make_pair(o->read_two, o->read_one)] = new_o;
      }
    }
  }

  for (auto it : best) {
    const Pair& p = it.first;
    Overlap* o = it.second;

    if (o->read_one == p.first and o->read_two == p.second) {
      overlaps->Add(new Overlap(*o));
    }
  }

  return overlaps.release();
}

}  // namespace overlap
