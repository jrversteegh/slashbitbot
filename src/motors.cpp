#include <cmath>
#include <array>
#include <algorithm>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#include "errors.h"
#include "sensors.h"
#include "storage.h"
#include "motors.h"

static constexpr int num_motors = 2;
static constexpr int num_pwms = 2 * num_motors;
static constexpr size_t num_converter_steps = 10;
static constexpr int converter_storage_id = 1;

using pwm_dt_specs = pwm_dt_spec[num_pwms];

struct Speed_to_setting_converter {
  Number max_speed{1.0};
  std::array<Number, num_converter_steps + 1> settings{};

  Number operator()(Number const& speed) {
    if (speed <= 0.0) {
      return 0.0;
    }
    else {
      auto ratio = speed / max_speed;
      if (ratio >= 1.0) {
        return settings[num_converter_steps];
      }
      else {
        auto trunc = std::trunc(num_converter_steps * ratio);
        auto index = std::lround(trunc);
        auto frac = (ratio - trunc) * num_converter_steps;
        return settings[index] * (1.0 - frac) + settings[index + 1] * frac;
      }
    }
  }
};

struct Speed_to_setting_converters {
  Speed_to_setting_converter left_forward{};
  Speed_to_setting_converter left_backward{};
  Speed_to_setting_converter right_forward{};
  Speed_to_setting_converter right_backward{};

  // The duplication for left and right motors
  // is annoying: TODO fix that at some point
  Number left(Number const& speed) {
    if (speed < 0.0) {
      return -left_backward(-speed);
    }
    else {
      return left_forward(speed);
    }
  }

  Number right(Number const& speed) {
    if (speed < 0.0) {
      return -right_backward(-speed);
    }
    else {
      return right_forward(speed);
    }
  }

  void calibrate(
      Speed_to_setting_converter& converter,
      Number const& max_speed,
      std::function<Number(void)> get_speed,
      std::function<void(Number)> set_setting
  ) {
    static constexpr int num_runs = 4;
    for (size_t i = 0; i <= num_converter_steps; ++i) {
      converter.settings[i] = 0.0;
    }
    for (int run = 1; run < num_runs + 1; ++run) {
      printk("Run: %d.\n", run);
      printk("Find start threshold.\n");
      converter.max_speed = max_speed;
      Number setting = 0.0;
      set_setting(setting);
      k_sleep(K_MSEC(2000));
      while (get_speed() == 0.0) {
        setting += 0.01;
        set_setting(setting);
        k_sleep(K_MSEC(2000));
      }
      converter.settings[0] += setting * (1.0 / num_runs);
      printk("Start threshold: %.2f\n", setting);
      for (size_t i = 1; i <= num_converter_steps; ++i) {
        auto speed_threshold = i * max_speed / num_converter_steps;
        Number speed = 0;
        while (speed < speed_threshold) {
          setting += setting < 0.28 ? 0.01 : 0.02;
          set_setting(setting);
          k_sleep(K_MSEC(1000));
          speed = get_speed();
          printk("Speed: %.3f / %.3f\n", speed, speed_threshold);
        }
        k_sleep(K_MSEC(1000));
        printk("Setting %2d: %.2f\n", i, setting);
        converter.settings[i] += setting * (1.0 / num_runs);
      }
    }
    for (size_t i = 0; i <= num_converter_steps; ++i) {
      printk("Table: %2d: %.3f\n", i, converter.settings[i]);
    }
    set_setting(0.0);
  }

  void calibrate() {
    printk("Calibrating motors.\n");
    k_sleep(K_SECONDS(5));
    set_motors(Motors_setting{1.0, 1.0});
    k_sleep(K_SECONDS(2));
    auto max_speeds_forward = get_wheel_speeds();
    set_motors(Motors_setting{-1.0, -1.0});
    k_sleep(K_SECONDS(2));
    auto max_speeds_backward = get_wheel_speeds();
    set_motors(Motors_setting{0.0, 0.0});
    auto max_speed = std::min(
        std::min(max_speeds_forward.left, max_speeds_forward.right),
        std::min(-max_speeds_backward.left, -max_speeds_backward.right)
    );
    printk("Max speed: %.3f\n", max_speed);
    printk("Left forward:\n");
    calibrate(left_forward, max_speed,
        [](){ return get_wheel_speeds().left; },
        [](Number const& s){ return set_motors(Motors_setting{s, 0.0}); }
    );
    printk("Right forward:\n");
    calibrate(right_forward, max_speed,
        [](){ return get_wheel_speeds().right; },
        [](Number const& s){ return set_motors(Motors_setting{0.0, s}); }
    );
    printk("Left backward:\n");
    calibrate(left_backward, max_speed,
        [](){ return -get_wheel_speeds().left; },
        [](Number const& s){ return set_motors(Motors_setting{-s, 0.0}); }
    );
    printk("Right backward:\n");
    calibrate(right_backward, max_speed,
        [](){ return -get_wheel_speeds().right; },
        [](Number const& s){ return set_motors(Motors_setting{0.0, -s}); }
    );
    store_object(converter_storage_id, *this);
  }
};

static Speed_to_setting_converters speed_to_setting{};

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
  if (!load_object(converter_storage_id, speed_to_setting)) {
    printk("Failed to load motor conversions. Not calibrated?\n");
  }
}

void calibrate_motors() {
  speed_to_setting.calibrate();
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

void set_motors(Motor_speeds const& speeds) {
  set_motors(Motors_setting{
      speed_to_setting.left(speeds.left),
      speed_to_setting.right(speeds.right)
  });
}
