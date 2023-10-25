#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "errors.h"
#include "sensors.h"

static const gpio_dt_spec left_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), left_wheel_gpios);
static const gpio_dt_spec right_wheel_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), right_wheel_gpios);

static Wheel_counters wheel_counters = {0, 0};

static struct gpio_callback left_wheel_callback;
static struct gpio_callback right_wheel_callback;

static void left_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        wheel_counters.left += 1;
}

static void right_wheel_irq(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        wheel_counters.right += 1;
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

void initialize_sensors() {
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
