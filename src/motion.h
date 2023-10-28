#ifndef SLASHBITBOT_MOTION_H__
#define SLASHBITBOT_MOTION_H__

#include <cmath>

#include "functions.h"

struct Vector {
  Number r{};
  Number heading{};
};

struct Path {
  Number r{};
  Number begin_heading{};
  Number end_heading{};
};

struct RelativeVector: public Vector {
};

struct RelativePath: public Path {
};

struct Motion {
  Number speed{};
  Number rot{};
  void update(Vector v, Number time_delta, Number rate) {
    speed = v.r * rate / time_delta + speed * (1 - rate);
    rot = v.heading * rate / time_delta + rot * (1 - rate);
  }
};

struct Locor {
  Number x{};
  Number y{};
  Number heading{};
  Locor& operator+=(Vector const& v) {
    heading = v.heading;
    x += std::cos(heading) * v.r;
    y += std::sin(heading) * v.r;
    return *this;
  }
  Locor& operator+=(RelativeVector const& rv) {
    heading = angle_mod(heading + rv.heading);
    x += std::cos(heading) * rv.r;
    y += std::sin(heading) * rv.r;
    return *this;
  }
};

inline Vector operator-(Locor const& locor1, Locor const& locor2) {
  auto dx = locor1.x - locor2.x;
  auto dy = locor1.y - locor2.y;
  return Vector{
    std::sqrt(dx * dx + dy * dy),
    std::atan2(dy, dx)
  };
}

void initialize_motion();

Locor get_locor();
void move(Vector const& v);
void move(RelativeVector const& rv);

#endif
