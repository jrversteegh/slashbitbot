#ifndef SLASHBITBOT_MOTION_H__
#define SLASHBITBOT_MOTION_H__

#include <cmath>

#include "functions.h"
#include "geometry.h"

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
  void flip_sign() {
    r = -r;
    heading = angle_mod_signed(heading + pi);
  }
};

struct RelativePath: public Path {
};

struct Motion {
  Number speed{};
  Number rot{};
  void update(Number const& speed, Number const& rot, Number rate) {
    update(Motion{speed, rot}, rate);
  }
  void update(Motion const& motion, Number rate) {
    speed = motion.speed * rate + speed * (1 - rate);
    rot = motion.rot * rate + rot * (1 - rate);
  }
};

struct Location {
  Number x{};
  Number y{};
};

struct Locor: public Location {
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
  Location& location() {
    return *this;
  }
};

inline Vector operator-(Location const& loc1, Location const& loc2) {
  auto dx = loc1.x - loc2.x;
  auto dy = loc1.y - loc2.y;
  return Vector{
    std::sqrt(dx * dx + dy * dy),
    angle_mod(std::atan2(dy, dx))
  };
}

inline RelativeVector operator-(Locor const& locor1, Locor const& locor2) {
  auto dx = locor1.x - locor2.x;
  auto dy = locor1.y - locor2.y;
  return RelativeVector{
    std::sqrt(dx * dx + dy * dy),
    angle_diff(std::atan2(dy, dx), locor2.heading)
  };
}

void initialize_motion();

Locor get_locor();
void move(Vector const& v);
void move(RelativeVector const& rv);

#endif
