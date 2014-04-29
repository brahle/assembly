#include <overlap/read.h>
#include <overlap/overlap.h>
#include <layout/unitigging.h>
#include <layout/layout_utils.h>
#include <cstdlib>
#include <ctime>
#include <memory>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <reads.afg> <overlaps.afg>\n", argv[0]);
    exit(0);
  }

  FILE *reads_file = fopen(argv[1], "r");
  FILE *overlaps_file = fopen(argv[2], "r");

  std::shared_ptr< overlap::ReadSet > reads(layout::ReadReadsAfg(reads_file));
  std::shared_ptr< overlap::OverlapSet > overlaps(
      layout::ReadOverlapsAfg(reads.get(), overlaps_file));

  clock_t start = clock();
  std::unique_ptr< layout::Unitigging > u(
      new layout::Unitigging(reads.get(), overlaps.get()));
  u->start();
  fprintf(
      stderr,
      "Unitigging finished in %.2lfs\n",
      (clock() - start) / static_cast<double>(CLOCKS_PER_SEC));

  int n50_value = layout::n50(u->contigs());
  fprintf(stderr, "n50 = %d\n", n50_value);

  fclose(overlaps_file);
  fclose(reads_file);
  return 0;
}
