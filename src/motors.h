#ifndef SLASHBITBOT_MOTORS_H__
#define SLASHBITBOT_MOTORS_H__

void initialize_motors();
void set_motors(float motor_left, float motor_right);

extern int left_motor_dir;
extern int right_motor_dir;

#endif
