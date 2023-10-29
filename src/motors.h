#ifndef SLASHBITBOT_MOTORS_H__
#define SLASHBITBOT_MOTORS_H__

#include "functions.h"

struct Motors_setting {
  Number left;
  Number right;
  auto operator<=>(Motors_setting const&) const = default;
};

struct Motor_speeds: public Motors_setting {
};

void initialize_motors();
void set_motors(Motors_setting const& setting);
void set_motors(Motor_speeds const& speeds);
void calibrate_motors();

extern int left_motor_dir;
extern int right_motor_dir;

#endif
