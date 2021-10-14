#include <centrifuge_test.hpp>
#include <cstdlib>

int main(void)
{
  CentrifugeTest test;
  test.cancel();
  test.start();
  while (test.isPollActive())
  {
    test.poll();
  }
  return EXIT_SUCCESS;
}