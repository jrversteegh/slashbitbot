#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "errors.h"
#include "motors.h"

static constexpr int num_motors = 2;
static constexpr int num_pwms = 2 * num_motors;

using pwm_dt_specs = pwm_dt_spec[num_pwms];

int left_motor_dir = 1;
int right_motor_dir = 1;

static pwm_dt_specs const pwms = {
  PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 0),
  PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 1),
  PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 0),
  PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 1)
};

void initialize_motors() {
  for (int i = 0; i < num_pwms; ++i) {
    if (!pwm_is_ready_dt(&pwms[i])) {
      error(0, "Error: Motor %s is not ready\n", pwms[i].dev->name);
      return;
    }
  }
  set_motors(Motors_setting{0.0, 0.0});
}

void set_motors(Motors_setting const& setting) {
  int ret = 0;
  if (setting.left > 0.0) {
    ret |= pwm_set_pulse_dt(&pwms[1], 0);
    ret |= pwm_set_pulse_dt(&pwms[0], PWM_NSEC(static_cast<int32_t>(setting.left * pwms[0].period)));
    left_motor_dir = 1;
  }
  else {
    ret |= pwm_set_pulse_dt(&pwms[0], 0);
    ret |= pwm_set_pulse_dt(&pwms[1], PWM_NSEC(static_cast<int32_t>(-setting.left * pwms[1].period)));
    left_motor_dir = -1;
  }
  if (setting.right > 0.0) {
    ret |= pwm_set_pulse_dt(&pwms[3], 0);
    ret |= pwm_set_pulse_dt(&pwms[2], PWM_NSEC(static_cast<int32_t>(setting.right * pwms[2].period)));
    right_motor_dir = 1;
  }
  else {
    ret |= pwm_set_pulse_dt(&pwms[2], 0);
    ret |= pwm_set_pulse_dt(&pwms[3], PWM_NSEC(static_cast<int32_t>(-setting.right * pwms[3].period)));
    right_motor_dir = -1;
  }
  if (ret != 0) {
    error(2, "Failed to set motor pwm");
  }
}

Number speed_to_setting_left(Number const& speed) {
  // TODO
  return speed;
}

Number speed_to_setting_right(Number const& speed) {
  // TODO
  return speed;
}

void set_motors(Motor_speeds const& speeds) {
  set_motors(Motors_setting{
      speed_to_setting_left(speeds.left),
      speed_to_setting_right(speeds.right)
  });
}
