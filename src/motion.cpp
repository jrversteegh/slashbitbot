#include <numbers>
#include <zephyr/kernel.h>

#include "motors.h"
#include "sensors.h"
#include "motion.h"

static constexpr Number pi = std::numbers::pi_v<Number>;
static constexpr Number width = 0.125;
static constexpr Number wheel_step = pi * 0.083 / 24;


static constexpr int stack_size = 512;
static constexpr int priority = 1;
static constexpr int clock_speed = 100; // Hz for control loop
static constexpr int clock_step = 1000 / clock_speed;
static constexpr Number motion_update_rate = 0.25;

static Locor actual_locor{};
static Locor target_locor{};
static Motion motion{};
static Motors_setting setting{};

static k_thread motion_control_thread;
static K_THREAD_STACK_DEFINE(motion_control_stack, stack_size);
static k_mutex motion_control_mutex;

struct Motion_control_mutex {
  Motion_control_mutex() {
    k_mutex_lock(&motion_control_mutex, K_FOREVER);
  }

  ~Motion_control_mutex() {
    k_mutex_unlock(&motion_control_mutex);
  }
};

void update_locor() {
  static Wheel_counters last_wheel_counters{};
  static int64_t last_time{};
  Wheel_counters wheel_counters = get_wheel_counters();
  if (wheel_counters != last_wheel_counters) {
    auto d_left = wheel_counters.left - last_wheel_counters.left;
    auto d_right = wheel_counters.right - last_wheel_counters.right;
    RelativeVector motion_step = {
      wheel_step * Number(0.5) * (d_left + d_right),
      wheel_step * (d_left - d_right) / width
    };
    actual_locor += motion_step;
    int time_delta = static_cast<int>(k_uptime_delta(&last_time));
    if (time_delta > 0) {
      motion.update(motion_step,  Number(0.001) * time_delta, motion_update_rate);
    }
    last_wheel_counters = wheel_counters;
  }
}

void control_motion(void*, void*, void*) {
  int i = 0;
  while (true) {
    {
      Motion_control_mutex lock{};

      // Motion updates can happen at a lower rate
      if (i % 10 == 0) {
        update_locor();
      }

      auto acceleration = get_acceleration();
      Vector vector = target_locor - actual_locor;
    }

    k_msleep(clock_step);
    ++i;
  }
}

void initialize_motion() {
  k_mutex_init(&motion_control_mutex);
  // Start motion control
  k_thread_create(&motion_control_thread, motion_control_stack, stack_size, control_motion, nullptr, nullptr, nullptr, priority, K_FP_REGS, K_NO_WAIT);
}

Locor get_locor() {
  Motion_control_mutex lock{};
  return actual_locor;
}

void move(Vector const& v) {
  Motion_control_mutex lock{};
  target_locor = actual_locor;
  target_locor += v;
}

void move(RelativeVector const& rv) {
  Motion_control_mutex lock{};
  target_locor = actual_locor;
  target_locor += rv;
}
