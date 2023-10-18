/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/display/mb_display.h>
#include <zephyr/sys/reboot.h>

static struct mb_image smile = MB_IMAGE(
    { 0, 1, 0, 1, 0 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 1 },
    { 0, 1, 1, 1, 0 }
);

static struct mb_image laugh1 = MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1 },
    { 0, 1, 1, 1, 0 }
);
static struct mb_image laugh2 = MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0 }
);

static struct mb_image sad1 = MB_IMAGE(
    { 1, 0, 0, 0, 0 },
    { 0, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
);
static struct mb_image sad2 = MB_IMAGE(
    { 0, 0, 0, 0, 1 },
    { 1, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
);
static struct mb_image sad3 = MB_IMAGE(
    { 1, 0, 0, 0, 1 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
);
static struct mb_image sad4 = MB_IMAGE(
    { 0, 0, 0, 0, 0 },
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 1 }
);

static struct mb_image woot = MB_IMAGE(
    { 1, 1, 0, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 1, 0, 0 }
);

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
  const struct device *const accelerometer = DEVICE_DT_GET_ONE(st_lis2dh);
  const struct device *const magnetometer = DEVICE_DT_GET_ONE(st_lis2mdl);
  const struct pwm_dt_spec motor0 = PWM_DT_SPEC_GET(DT_NODELABEL(motor0));
  const struct pwm_dt_spec motor1 = PWM_DT_SPEC_GET(DT_NODELABEL(motor1));

  struct mb_display *disp = mb_display_get();

  mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 4000, &smile, 1);

  k_sleep(K_SECONDS(4));

  if (!device_is_ready(accelerometer)) {
    printk("Device %s is not ready\n", accelerometer->name);
    mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 2000, &sad1, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  if (!device_is_ready(magnetometer)) {
    printk("Device %s is not ready\n", magnetometer->name);
    mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 2000, &sad2, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  if (read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ) < 0) {
    printk("Couldn't read acceleration");
    mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 2000, &woot, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }
  printk("Acceleration: %f, %f ,%f\n", x, y, z);


  if (!pwm_is_ready_dt(&motor0)) {
    printk("Error: Motor %s is not ready\n", motor0.dev->name);
    mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 2000, &sad3, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  if (!pwm_is_ready_dt(&motor1)) {
    printk("Error: Motor %s is not ready\n", motor1.dev->name);
    mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 2000, &sad4, 1);
    k_sleep(K_SECONDS(2));
    sys_reboot(SYS_REBOOT_COLD);
    return 1;
  }

  int pulse = 0;
  while (true) {
    pwm_set_pulse_dt(&motor0, PWM_USEC(((pulse * 400) % 40000)));
    pwm_set_pulse_dt(&motor1, PWM_USEC(((pulse * 400) + 20000) % 40000));
    if (pulse % 2 == 0) {
      mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 1000, &laugh1, 1);
    }
    else {
      mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, 1000, &laugh2, 1);
    }
    ++pulse;
    k_sleep(K_SECONDS(1));
    if (pulse > 100) {
      sys_reboot(SYS_REBOOT_COLD);
    }
  }

  return 0;
}
