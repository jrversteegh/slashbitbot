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

static double x = 0.0;
static double y = 0.0;
static double z = 0.0;

static int32_t read_sensor(const struct device *sensor,
    enum sensor_channel channel)
{
  struct sensor_value val[3];
  int32_t ret = 0;

  ret = sensor_sample_fetch(sensor);
  if (ret < 0 && ret != -EBADMSG) {
    printk("Sensor sample update error\n");
    goto end;
  }

  ret = sensor_channel_get(sensor, channel, val);
  if (ret < 0) {
    printk("Cannot read sensor channels\n");
    goto end;
  }

  x = sensor_value_to_double(&val[0]);
  y = sensor_value_to_double(&val[1]);
  z = sensor_value_to_double(&val[2]);
end:
  return ret;
}


int main(void) {
  printk("SlashBitBot initializing...\n");
  initialize_sensors();
  initialize_motors();
  const struct device *const accelerometer = DEVICE_DT_GET(DT_ALIAS(accel0));
  const struct device *const magnetometer = DEVICE_DT_GET(DT_ALIAS(magn0));

  // Upon power on this will happen (not sure why). Will be fine after MCU reset
  if (!device_is_ready(accelerometer) || !device_is_ready(magnetometer)) {
    printk("Device %s is not ready\n", accelerometer->name);
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  show_smile(4);
  k_sleep(K_SECONDS(4));

  int i = 0;

  printk("SlashBitBot running...\n");
  set_motors(1.0, 1.0);
  while (true) {
    if (read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ) < 0) {
      error(1, "Couldn't read acceleration");
    }
    printk("Acceleration: %.4f, %.4f, %.4f\n", x, y, z);

    //set_motors(sin(0.07 * i), cos(0.07 * i));
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
