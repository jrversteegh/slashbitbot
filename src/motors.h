#ifndef SLASHBITBOT_MOTORS_H__
#define SLASHBITBOT_MOTORS_H__

#include "functions.h"

struct Motors_setting {
  Number left;
  Number right;
};

void initialize_motors();
void set_motors(Motors_setting const& setting);

extern int left_motor_dir;
extern int right_motor_dir;

#endif
