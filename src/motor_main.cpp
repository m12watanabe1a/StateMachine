#include "motor.hpp"
#include <cstdlib>

int main(void)
{
  auto motor = std::make_shared<Motor>();
  auto data = std::make_shared<MotorData>();
  data->speed = 100;
  motor->setSpeed(data);

  data->speed = 200;
  motor->setSpeed(data);

  motor->halt();

  return EXIT_SUCCESS;
}
