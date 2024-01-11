#include "package.hpp"
#include "test.hpp"

int main(void)
{
  int len = Package::GetVersion().length();
  assert(len >= 5 && len < 10);
  assert(Package::GetName().compare("NetPanzer") == 0);

  return 0;
}
