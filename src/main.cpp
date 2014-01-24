#include <cstdio>
#include <cstdlib>
#include <memory>

#include "data.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    exit(1);
  }

  FILE* in = fopen(argv[1], "r");
  std::unique_ptr<overlap::ReadSet> readset(overlap::ReadFasta(in));

  for (uint32_t rix = 0; rix < readset->size(); ++rix) {
    printf("%s\n", readset->get(rix)->data());
  }

  return 0;
}
