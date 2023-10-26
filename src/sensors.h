#ifndef SLASHBITBOT_SENSORS_H__
#define SLASHBITBOT_SENSORS_H__

struct Vector3 {
  float x;
  float y;
  float z;
};

struct Wheel_counters {
  int left;
  int right;
};

struct Wheel_sensors {
  bool left;
  bool right;
};

void initialize_sensors();
Wheel_counters get_wheel_counters();
Wheel_sensors get_wheel_sensors();
Vector3 get_acceleration();

#endif
