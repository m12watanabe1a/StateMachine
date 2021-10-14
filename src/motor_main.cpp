#include "motor.hpp"
#include <cstdlib>

int main(void)
{
  Motor motor;
  auto data = new MotorData();
  data->speed = 100;
  motor.setSpeed(data);

  auto data2 = new MotorData();
  data2->speed = 200;
  motor.setSpeed(data2);

  motor.halt();
  // motor.Halt();

  return EXIT_SUCCESS;
}
