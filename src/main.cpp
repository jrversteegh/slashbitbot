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
#include "storage.h"
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

static const struct gpio_dt_spec sw0_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec sw1_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

static void button_pressed(const device *dev, gpio_callback *cb, uint32_t pins) {
  if (pins & BIT(sw0_gpio.pin)) {
    printk("A pressed\n");
  } else {
    printk("B pressed\n");
  }
}

void print_sensors() {
  auto acceleration = get_acceleration();
  printk("Acceleration: %.4f, %.4f, %.4f\n", acceleration.x, acceleration.y, acceleration.z);

  auto mag_field = get_magfield();
  printk("Mag field: %.4f, %.4f, %.4f\n", mag_field.x, mag_field.y, mag_field.z);

  auto counters = get_wheel_counters();
  printk("Wheel left: %d, right: %d\n", counters.left, counters.right);
}

static void initialize_buttons() {
  static struct gpio_callback button_cb_data;

  if (!gpio_is_ready_dt(&sw0_gpio) || !gpio_is_ready_dt(&sw1_gpio)) {
    error(0, "Button io not ready.");
    return;
  }

  gpio_pin_configure_dt(&sw0_gpio, GPIO_INPUT);
  gpio_pin_configure_dt(&sw1_gpio, GPIO_INPUT);

  gpio_pin_interrupt_configure_dt(&sw0_gpio, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_pin_interrupt_configure_dt(&sw1_gpio, GPIO_INT_EDGE_TO_ACTIVE);

  gpio_init_callback(&button_cb_data, button_pressed,
      BIT(sw0_gpio.pin) | BIT(sw1_gpio.pin));

  gpio_add_callback(sw0_gpio.port, &button_cb_data);
}

int main(void) {
  printk("SlashBitBot initializing...\n");
  initialize_buttons();
  initialize_storage();
  initialize_sensors();
  initialize_motors();

  // When button A pressed at startup: calibrate motors
  if (gpio_pin_get_dt(&sw0_gpio)) {
    calibrate_motors();
  }

  // Start motion control
  initialize_motion();

  show_smile(4);
  k_sleep(K_SECONDS(4));

  printk("SlashBitBot running...\n");
  //motor_non_linearity_test();
  int i = 0;
  while (true) {
    if (gpio_pin_get_dt(&sw1_gpio)) {
      print_sensors();
    }

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
