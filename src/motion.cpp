#include <zephyr/kernel.h>

#include "functions.h"
#include "motors.h"
#include "sensors.h"
#include "motion.h"

static constexpr int stack_size = 512;
static constexpr int priority = -2;
static constexpr int clock_speed = 100; // Hz for control loop
static constexpr int clock_step = 1000 / clock_speed;

// Motion model parameters
static constexpr Number motion_update_rate = 0.25;
static constexpr Number max_backup_distance = 0.1;


static Locor actual_locor{};
static Locor target_locor{};
static Motion motion{};
static Motor_speeds motor_speeds{};

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

void update_locor_motion() {
  static Wheel_counters last_wheel_counters{};
  Wheel_counters wheel_counters = get_wheel_counters();
  if (wheel_counters != last_wheel_counters) {
    auto delta = wheel_counters - last_wheel_counters;
    RelativeVector motion_step = {
      wheel_step * Number(0.5) * (delta.left + delta.right),
      wheel_step * (delta.left - delta.right) / width
    };
    actual_locor += motion_step;
    auto wheel_speeds = get_wheel_speeds();
    motion.update(
        0.5 * (wheel_speeds.left + wheel_speeds.right),
        (wheel_speeds.left - wheel_speeds.right) / width,
        motion_update_rate
    );
    last_wheel_counters = wheel_counters;
  }
}

void control_motors(Motor_speeds const& speeds) {
  if (speeds != motor_speeds) {
    set_motors(speeds);
    motors_speeds = setting;
  }
}

void control_motion(void*, void*, void*) {
  int i = 0;
  while (true) {
    {
      Motion_control_mutex lock{};

      // Motion updates can happen at a lower rate
      if (i % 10 == 0) {
        update_locor_motion();
      }

      auto acceleration = get_acceleration();

      // Fall detection...
      if (std::fabs(acceleration.x) > Number(6.0)) {
        // Stop motors and do nothing when tipped over
        set_motors(Motors_setting{0.0, 0.0});
      }
      else {
        RelativeVector vector = target_locor - actual_locor;
        // Consider whether to backup or turn around
        if (std::fabs(vector.heading) > pi / 2 && vector.r < max_backup_distance) {
          vector.flip_sign();
        }
      }
    }
    k_msleep(clock_step);
    ++i;
  }
}

void initialize_motion() {
  k_mutex_init(&motion_control_mutex);
  // Start motion control
  k_thread_create(&motion_control_thread, motion_control_stack, stack_size,
      control_motion, nullptr, nullptr, nullptr, priority, K_FP_REGS, K_NO_WAIT);
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
