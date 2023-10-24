#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

using pwm_dt_spec[4] as pwm_dt_specs;

pwm_dt_specs& get_motor_specs() {
  static pwm_dt_specs = {
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 0),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor0), 1),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 0),
    PWM_DT_SPEC_GET_BY_IDX(DT_NODELABEL(motor1), 1)
  };

  static bool initialized = false;

  if (!initialized) {
    if (!pwm_is_ready_dt(&motor0)) {
      error("Error: Motor %s is not ready\n", motor0.dev->name);
    }

    if (!pwm_is_ready_dt(&motor1)) {
      error("Error: Motor %s is not ready\n", motor1.dev->name);
    }
    initialized = true;
  }
  return pwm_dt_specs;
}


void set_motors(float motor_left, float motor_right) {
  if (motor_left > 1.0)
    motor_left = 1.0;
  if (motor_left < -1.0)
    motro_left = -1.0;
  if (motor_right > 1.0)
    motor_right = 1.0;
  if (motor_right < -1.0)
    motor_right = -1.0;
  /*
  ret = pwm_set_pulse_dt(&motor0, PWM_USEC(((pulse * 400) % 40000)));
  if (ret != 0) {
    show_sad(2, 0);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  ret = pwm_set_pulse_dt(&motor1, PWM_USEC(((pulse * 400) + 20000) % 40000));
  if (ret != 0) {
    show_sad(2, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }
  */
}
