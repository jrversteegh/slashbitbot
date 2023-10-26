/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

#include "display.h"
#include "errors.h"
#include "sensors.h"
#include "motors.h"

int main(void) {
  printk("SlashBitBot initializing...\n");
  initialize_sensors();
  initialize_motors();

  show_smile(4);
  k_sleep(K_SECONDS(4));

  printk("SlashBitBot running...\n");
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
