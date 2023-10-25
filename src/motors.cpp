#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "errors.h"

static constexpr int num_motors = 2;
static constexpr int num_pwms = 2 * num_motors;

extern "C" {

using pwm_dt_specs = pwm_dt_spec[num_pwms];

pwm_dt_specs const& get_motor_pwms() {
  static pwm_dt_specs const pwms = {
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 0),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 1),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 0),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 1)
  };
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < num_pwms; ++i) {
      if (!pwm_is_ready_dt(&pwms[i])) {
        error(0, "Error: Motor %s is not ready\n", pwms[i].dev->name);
      }
    }
    initialized = true;
  }
  return pwms;
}

}


void set_motors(float motor_left, float motor_right) {
  if (motor_left > 1.0)
    motor_left = 1.0;
  if (motor_left < -1.0)
    motor_left = -1.0;
  if (motor_right > 1.0)
    motor_right = 1.0;
  if (motor_right < -1.0)
    motor_right = -1.0;

  auto motor_pwms = get_motor_pwms();

  int ret = 0;
  if (motor_left > 0.0) {
    ret |= pwm_set_pulse_dt(&motor_pwms[1], 0);
    ret |= pwm_set_pulse_dt(&motor_pwms[0], PWM_NSEC(static_cast<int32_t>(motor_left * motor_pwms[0].period)));
  }
  else {
    ret |= pwm_set_pulse_dt(&motor_pwms[0], 0);
    ret |= pwm_set_pulse_dt(&motor_pwms[1], PWM_NSEC(static_cast<int32_t>(-motor_left * motor_pwms[1].period)));
  }
  if (motor_right > 0.0) {
    ret |= pwm_set_pulse_dt(&motor_pwms[3], 0);
    ret |= pwm_set_pulse_dt(&motor_pwms[2], PWM_NSEC(static_cast<int32_t>(motor_right * motor_pwms[2].period)));
  }
  else {
    ret |= pwm_set_pulse_dt(&motor_pwms[2], 0);
    ret |= pwm_set_pulse_dt(&motor_pwms[3], PWM_NSEC(static_cast<int32_t>(-motor_right * motor_pwms[3].period)));
  }
  if (ret != 0) {
    error(2, "Failed to set motor pwm");
  }
}
