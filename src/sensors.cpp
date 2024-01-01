#include <type_traits>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "errors.h"
#include "sensors.h"
#include "motors.h"
#include "geometry.h"

static constexpr Number wheel_speed_update_factor = 0.25;

static gpio_dt_spec const left_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), left_wheel_gpios);
static  gpio_dt_spec const right_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), right_wheel_gpios);
static device const* const accelerometer = DEVICE_DT_GET(DT_ALIAS(accel0));
// Compass is useless because of the nearby motors, but initialize anyway
static device const* const magnetometer = DEVICE_DT_GET(DT_ALIAS(magn0));

static int left_wheel_counter = 0;
static int right_wheel_counter = 0;
static Number left_wheel_speed = 0;
static Number right_wheel_speed = 0;

static struct gpio_callback left_wheel_callback;
static struct gpio_callback right_wheel_callback;

static int64_t last_left_time = 0;
static int64_t last_right_time = 0;

static void left_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  auto delta = k_uptime_get() - last_left_time;
  // Debounce by requiring more than 10ms between counts
  if (delta > 10) {
    left_wheel_counter += left_motor_dir;
    left_wheel_speed = (1 - wheel_speed_update_factor) * left_wheel_speed
                     + wheel_speed_update_factor * wheel_step * left_motor_dir * 1000.0 / delta;
    last_left_time += delta;
  }
}

static void right_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  auto delta = k_uptime_get() - last_right_time;
  // Debounce by requiring more than 10ms between counts
  if (delta > 10) {
    right_wheel_counter += right_motor_dir;
    right_wheel_speed = (1 - wheel_speed_update_factor) * right_wheel_speed
                      + wheel_speed_update_factor * wheel_step * right_motor_dir * 1000.0 / delta;
    last_right_time += delta;
  }
}

Wheel_counters get_wheel_counters() {
  return Wheel_counters({left_wheel_counter, right_wheel_counter});
}

Wheel_speeds get_wheel_speeds() {
  auto now = k_uptime_get();
  if (now - last_left_time > 900) {
    left_wheel_speed = 0.0;
  }
  if (now - last_right_time > 900) {
    right_wheel_speed = 0.0;
  }
  return Wheel_speeds({left_wheel_speed, right_wheel_speed});
}

Wheel_sensors get_wheel_sensors() {
  Wheel_sensors result;
  result.left = gpio_pin_get_dt(&left_wheel_gpio);
  result.right = gpio_pin_get_dt(&right_wheel_gpio);
  return result;
}

struct None {};

template <typename T>
inline T sensor_value_to(sensor_value const& value) {
  static_assert(std::is_same_v<T, None>, "Conversion not supported");
  return 0;
}

template <>
inline float sensor_value_to<float>(sensor_value const& value) {
  return sensor_value_to_float(&value);
}

template <>
inline double sensor_value_to<double>(sensor_value const& value) {
  return sensor_value_to_double(&value);
}

static Vector3 read_sensor(device const* sensor, sensor_channel channel) {
  struct sensor_value values[3];
  Vector3 result{};

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

  result.x = -sensor_value_to<decltype(result.x)>(values[2]);
  result.y = sensor_value_to<decltype(result.y)>(values[0]);
  result.z = sensor_value_to<decltype(result.z)>(values[1]);
  return result;
}

Vector3 get_acceleration() {
    return read_sensor(accelerometer, SENSOR_CHAN_ACCEL_XYZ);
}

Vector3 get_magfield() {
    return read_sensor(magnetometer, SENSOR_CHAN_MAGN_XYZ);
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
