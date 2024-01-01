#ifndef SLASHBITBOT_MOTORS_H__
#define SLASHBITBOT_MOTORS_H__

#include "functions.h"
#include "geometry.h"

struct Motors_setting {
  Number left;
  Number right;
  auto operator<=>(Motors_setting const&) const = default;
};

struct Motor_speeds: public Motors_setting {
  Number get_speed() {
    return 0.5 * (left + right);
  }
  Number get_rot() {
    return (left - right) / width;
  }
};

void initialize_motors();
void set_motors(Motors_setting const& setting);
void set_motors(Motor_speeds const& speeds);
void calibrate_motors();

extern int left_motor_dir;
extern int right_motor_dir;

#endif
