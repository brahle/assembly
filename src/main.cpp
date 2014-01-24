#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#include "data.h"
#include "sort.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    exit(1);
  }

  FILE* in = fopen(argv[1], "r");
  std::unique_ptr<overlap::ReadSet> read_set(overlap::ReadFasta(in));
  std::vector<uint32_t> read_order = overlap::RadixStringOrder(*read_set, 256);

  for (uint32_t rix = 0; rix < read_set->size(); ++rix) {
    printf("%s\n", read_set->get(read_order[rix])->data());
  }

  return 0;
}
