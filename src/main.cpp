#include "motor.hpp"
#include <cstdlib>

int main(void)
{
  Motor motor;
  auto data = std::make_shared<MotorData>();
  data->speed = 100;
  motor.SetSpeed(data);

  auto data2 = std::make_shared<MotorData>();
  data2->speed = 200;
  motor.SetSpeed(data2);

  motor.Halt();
  // motor.Halt();

  return EXIT_SUCCESS;
}
