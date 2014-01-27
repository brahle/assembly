#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

#include "fm_index.h"
#include "overlap.h"
#include "read.h"
#include "sort.h"
#include "suffix_array.h"
#include "suffix_filter.h"
#include "util.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    exit(1);
  }

  double error_rate = 0.01;
  size_t min_read_size = 50;
  size_t min_overlap_size = 25;


  time_t start = clock(), prev, curr;

  printf("Reading genome data.\n");
  prev = clock();
  FILE* in = fopen(argv[1], "r");
  std::unique_ptr<overlap::ReadSet> read_set(overlap::ReadFasta(in, min_read_size));
  fclose(in);
  curr = clock();
  printf("  %d\n", read_set->size());
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);
  const size_t num_reads = read_set->size();

  printf("Building BWT.\n");
  prev = clock();
  overlap::SaisSACA saca;
  const overlap::String* bwt = saca.BuildBWT(*read_set, 4);
  if (bwt == nullptr) {
    exit(1);
  }
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Building FM-index.\n");
  prev = clock();
  overlap::BucketedFMIndex fmi(bwt, 4, 32);
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Sorting reads.\n"); fflush(stdout);
  prev = clock();
  overlap::UintArray read_order = overlap::STLStringOrder(*read_set);
  curr = clock();
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Finding candidates.\n");
  prev = clock();
  overlap::BFSSuffixFilter sufter(fmi, read_order, error_rate, min_overlap_size);
  overlap::OverlapSet candidates_raw(num_reads * num_reads / 2);
  std::unordered_set<uint32_t> contained;
  for (uint32_t read_idx = 0; read_idx < num_reads; ++read_idx) {
    if (sufter.FindCandidates(*read_set->Get(read_idx), &candidates_raw)) {
      contained.insert(read_idx);
    }
  }
  curr = clock();
  printf("  %d\n", candidates_raw.size());
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  printf("Filtering candidates.\n");
  overlap::OverlapSet candidates(candidates_raw.size() / 2);
  overlap::FilterCandidates(contained, candidates_raw, &candidates);
  curr = clock();
  printf("  %d\n", candidates.size());
  printf("  %.3fs\n", ((double)curr - prev) / CLOCKS_PER_SEC);

  FILE* fout = fopen(argv[2], "w");
  for (uint32_t oid = 0; oid < candidates.size(); ++oid) {
    const overlap::Overlap* o = candidates[oid];
    fprintf(fout, "%d %d %d %d\n", o->read_one, o->read_two, o->len_one, o->score);
  }
  fclose(fout);

  printf("Total: ");
  printf("%.3fs\n", ((double)curr - start) / CLOCKS_PER_SEC);

  return 0;
}
