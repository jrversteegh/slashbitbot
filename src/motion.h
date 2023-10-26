#ifndef SLASHBITBOT_MOTION_H__
#define SLASHBITBOT_MOTION_H__

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

struct Locor {
  Number x{};
  Number y{};
  Number heading{};
  Locor& operator+=(Vector const& v) {
    heading = v.heading;
    x += cos(heading) * v.r;
    y += sin(heading) * v.r;
    return *this;
  }
  Locor& operator+=(RelativeVector const& rv) {
    heading = angle_mod(heading + rv.heading);
    x += cos(heading) * rv.r;
    y += sin(heading) * rv.r;
    return *this;
  }
};

void initialize_motion();

Locor get_locor();
void move(Vector const& v);
void move(RelativeVector const& rv);

#endif
