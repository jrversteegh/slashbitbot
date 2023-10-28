/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

#include "display.h"
#include "errors.h"
#include "sensors.h"
#include "motors.h"
#include "motion.h"

void motor_non_linearity_test() {
  printk("Test motors non-linearity:\n");
  set_motors(Motors_setting{0.0, 0.0});
  auto wheel_counters = get_wheel_counters();
  for (Number f = 0.0; f <= 4.0; f += 0.05) {
    auto s = f;
    if (f > 3.0) {
      s -= 4.0;
    }
    else if (f > 1.0) {
      s = 2.0 - f;
    }
    set_motors(Motors_setting{s, s});
    k_sleep(K_SECONDS(5));
    auto wheel_delta = wheel_counters.delta(get_wheel_counters());
    printk("%5.2f, %3d, %3d\n", s, wheel_delta.left, wheel_delta.right);
  }
}

int main(void) {
  printk("SlashBitBot initializing...\n");
  initialize_sensors();
  initialize_motors();
  //initialize_motion();

  show_smile(4);
  k_sleep(K_SECONDS(4));

  printk("SlashBitBot running...\n");
  //motor_non_linearity_test();
  int i = 0;
  while (true) {
    auto acceleration = get_acceleration();
    printk("Acceleration: %.4f, %.4f, %.4f\n", acceleration.x, acceleration.y, acceleration.z);

    auto counters = get_wheel_counters();
    printk("Wheel left: %d, right: %d\n", counters.left, counters.right);

    if (i % 2 == 0) {
      show_laugh(1, 0);
    }
    else {
      show_laugh(1, 1);
    }
    ++i;
    k_sleep(K_SECONDS(1));
  }

  printk("SlashBitBot done.\n");
  return 0;
}
