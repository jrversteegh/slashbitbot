#ifndef SLASHBITBOT_SENSORS_H__
#define SLASHBITBOT_SENSORS_H__

#include "functions.h"
#include "geometry.h"

struct Vector3 {
  Number x;
  Number y;
  Number z;
};


struct Wheel_counters {
  int left;
  int right;

  auto operator<=>(const Wheel_counters&) const = default;
  Wheel_counters& operator-=(Wheel_counters const& other) {
    left -= other.left;
    right -= other.right;
    return *this;
  }
  Wheel_counters delta(Wheel_counters const& other) {
    Wheel_counters result = other;
    result -= *this;
    *this = other;
    return result;
  }
};

inline Wheel_counters operator-(Wheel_counters const& counters1, Wheel_counters const& counters2) {
  return Wheel_counters({counters1.left - counters2.left, counters1.right - counters2.right});
}

struct Wheel_sensors {
  bool left;
  bool right;
};

struct Wheel_speeds {
  Number left;
  Number right;
  Number get_speed() {
    return 0.5 * (left + right);
  }
  Number get_rot() {
    return (left - right) / width;
  }
};

void initialize_sensors();
Wheel_counters get_wheel_counters();
Wheel_sensors get_wheel_sensors();
Wheel_speeds get_wheel_speeds();
Vector3 get_acceleration();
Vector3 get_magfield();

#endif
