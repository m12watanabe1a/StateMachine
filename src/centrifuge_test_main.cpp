#include <centrifuge_test.hpp>
#include <cstdlib>

int main(void)
{
  auto test = std::make_shared<CentrifugeTest>();
  test->cancel();
  test->start();
  while (test->isPollActive())
  {
    test->poll();
  }
  return EXIT_SUCCESS;
}