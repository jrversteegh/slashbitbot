#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "errors.h"
#include "sensors.h"
#include "motors.h"

static gpio_dt_spec const left_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), left_wheel_gpios);
static  gpio_dt_spec const right_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), right_wheel_gpios);
static device const* const accelerometer = DEVICE_DT_GET(DT_ALIAS(accel0));
// Compass is useless because of the nearby motors, but initialize anyway
static device const* const magnetometer = DEVICE_DT_GET(DT_ALIAS(magn0));

static Wheel_counters wheel_counters = {0, 0};

static struct gpio_callback left_wheel_callback;
static struct gpio_callback right_wheel_callback;

static void left_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  wheel_counters.left += left_motor_dir;
}

static void right_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  wheel_counters.right += right_motor_dir;
}

Wheel_counters get_wheel_counters() {
  return wheel_counters;
}

Wheel_sensors get_wheel_sensors() {
  Wheel_sensors result;
  result.left = gpio_pin_get_dt(&left_wheel_gpio);
  result.right = gpio_pin_get_dt(&right_wheel_gpio);
  return result;
}

static Vector read_sensor(device const* sensor, sensor_channel channel) {
  struct sensor_value values[3];
  Vector result{};

  auto ret = sensor_sample_fetch(sensor);
  if (ret < 0 && ret != -EBADMSG) {
    error(2, "Sensor sample update error.");
    return result;
  }

  ret = sensor_channel_get(sensor, channel, values);
  if (ret < 0) {
    error(2, "Cannot read sensor channels.");
    return result;
  }

  result.x = sensor_value_to_double(&values[0]);
  result.y = sensor_value_to_double(&values[1]);
  result.z = sensor_value_to_double(&values[2]);
  return result;
}

Vector get_acceleration() {
    return read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ);
}

void initialize_sensors() {
  // Upon power on this will happen (not sure why). Will be fine after MCU reset
  if (!device_is_ready(accelerometer)) {
    printk("Accelerometer not ready.\n");
    sys_reboot(SYS_REBOOT_COLD);
    return;
  }

  if (!device_is_ready(magnetometer)) {
    error(2, "Magnetometer not ready.");
    return;
  }

  if (!gpio_is_ready_dt(&left_wheel_gpio) || !gpio_is_ready_dt(&right_wheel_gpio)) {
    error(2, "Left/Right wheel GPIO not ready.");
    return;
  }

  gpio_pin_configure_dt(&left_wheel_gpio, GPIO_INPUT);
  gpio_pin_configure_dt(&right_wheel_gpio, GPIO_INPUT);

  gpio_pin_interrupt_configure_dt(&left_wheel_gpio, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_pin_interrupt_configure_dt(&right_wheel_gpio, GPIO_INT_EDGE_TO_ACTIVE);

  gpio_init_callback(&left_wheel_callback, left_wheel_irq, BIT(left_wheel_gpio.pin));
  gpio_add_callback(left_wheel_gpio.port, &left_wheel_callback);

  gpio_init_callback(&right_wheel_callback, right_wheel_irq, BIT(right_wheel_gpio.pin));
  gpio_add_callback(right_wheel_gpio.port, &right_wheel_callback);
}
