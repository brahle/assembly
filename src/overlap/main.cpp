#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "fm_index.h"
#include "read.h"
#include "sort.h"
#include "suffix_array.h"
#include "suffix_filter.h"
#include "util.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    exit(1);
  }

  double error_rate = 0.10;
  size_t min_read_size = 60;
  size_t min_overlap_size = 40;

  FILE* in = fopen(argv[1], "r");

  time_t start = clock(), prev, curr;

  printf("Reading genome data.\n");
  prev = clock();
  std::unique_ptr<overlap::ReadSet> read_set(
      overlap::ReadFasta(in, min_read_size));
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Building BWT.\n");
  prev = clock();
  std::unique_ptr<overlap::SACA> saca(
      new overlap::SaisSACA());
  overlap::String* bwt = saca->BuildBWT(*read_set, 4);
  if (bwt == nullptr) {
    exit(1);
  }

  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Building FM-index.\n"0);
  prev = clock();
  std::unique_ptr<overlap::FMIndex> fmi(
      new overlap::BucketedFMIndex(bwt, 4, 128));
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Sorting reads.\n"); fflush(stdout);
  prev = clock();
  overlap::UintArray read_order = overlap::RadixStringOrder(*read_set, 4);
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Finding candidates.\n");
  prev = clock();
  std::unique_ptr<overlap::SuffixFilter> sufter(
      new overlap::BFSSuffixFilter(*fmi, read_order, error_rate, min_overlap_size));

  for (uint32_t read_idx = 0; read_idx < read_set->size(); ++read_idx) {
  }
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Total: ");
  printf("%.3fs\n", ((double)curr - start) / CLOCKS_PER_SEC);

  return 0;
}
