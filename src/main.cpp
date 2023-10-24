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
#include <zephyr/sys/reboot.h>

#include "display.h"
#include "errors.h"
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

  if (read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ) < 0) {
    printk("Couldn't read acceleration");
    show_woot(2);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }
  printk("Acceleration: %f, %f ,%f\n", x, y, z);


  int pulse = 0;
  int ret;

  while (true) {


    if (pulse % 2 == 0) {
      show_laugh(1, 0);
    }
    else {
      show_laugh(1, 1);
    }
    ++pulse;
    k_sleep(K_SECONDS(1));
    if (pulse > 100) {
      sys_reboot(SYS_REBOOT_COLD);
    }
  }

  return 0;
}
