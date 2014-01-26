#include <layout/unitigging.h>

int main() {
  overlap::ReadSet rs(10);
  overlap::OverlapSet os(100);
  layout::Unitigging u(&rs, &os);

  return 0;
}
