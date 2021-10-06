#include "motor.hpp"
#include <cstdlib>

int main(void)
{
  Motor motor;
  MotorData *data = new MotorData();
  data->speed = 100;
  motor.SetSpeed(data);

  MotorData *data2 = new MotorData();
  data2->speed = 200;
  motor.SetSpeed(data2);

  motor.Halt();
  motor.Halt();

  return EXIT_SUCCESS;
}
