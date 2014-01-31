#include "layout/layout_utils.h"
#include <cstdio>
#include <cstring>

namespace layout {

overlap::Read* ReadOneReadAfg(FILE *fd) {
    int id;
    int length;
    char c;
    const uint8_t *data = new uint8_t[1];
    fscanf(fd, " iid:%d eid:%*s", &id);
    do {
      fscanf(fd, " %c", &c);
    } while (c != '.');
    do {
      fscanf(fd, " %c", &c);
    } while (c != '.');
    fscanf(fd, " frg:%*d clr:%*d,%d", &length);
    return new overlap::Read(data, length, (id-1), id);
}

overlap::ReadSet* ReadReadsAfg(FILE *fd) {
  clock_t start = clock();
  auto read_set = new overlap::ReadSet(10000);
  int i = 0;
  char buff[1 << 17];
  while (fscanf(fd, " %s", buff) == 1) {
    if (!strcmp(buff, "{RED")) {
      read_set->Add(ReadOneReadAfg(fd));
    }
  }
  fprintf(
      stderr,
      "Reads read in %.2lfs\n",
      (clock() - start)/static_cast<double>(CLOCKS_PER_SEC));
  return read_set;
}

overlap::OverlapSet* ReadOverlapsAfg(overlap::ReadSet* read_set, FILE *fd) {
  clock_t start = clock();
  overlap::OverlapSet* overlap_set = new overlap::OverlapSet(10000);
  char type;
  int read_one;
  int read_two;
  int score;
  int hang_one;
  int hang_two;
  int len_one;
  int len_two;
  while (fscanf(
             fd,
             " {OVL adj:%c rds:%d,%d scr:%d ahg:%d bhg:%d }",
             &type,
             &read_one,
             &read_two,
             &score,
             &hang_one,
             &hang_two) == 6) {
    --read_one;
    --read_two;
    if (hang_one >= 0) {
      len_one = (*read_set)[read_one]->size() - abs(hang_one);
    } else {
      len_one = std::min(
          (*read_set)[read_one]->size(),
          (*read_set)[read_two]->size() - abs(hang_one));
    }
    if (hang_two >= 0) {
      len_two = (*read_set)[read_two]->size() - abs(hang_two);
    } else {
      len_two = std::min(
          (*read_set)[read_two]->size(),
          (*read_set)[read_one]->size() - abs(hang_two));
    }
    overlap_set->Add(new overlap::Overlap(
        read_one,
        read_two,
        len_one,
        len_two,
        overlap::Overlap::Type::EB,
        0));
  }
  fprintf(
      stderr,
      "Overlaps read in %.2lfs\n",
      (clock() - start)/static_cast<double>(CLOCKS_PER_SEC));
  return overlap_set;
}

};  // namespace layout

