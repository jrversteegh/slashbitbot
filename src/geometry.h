#ifndef SLASHBITBOT_GEOMETRY_H__
#define SLASHBITBOT_GEOMETRY_H__

#include "functions.h"

static constexpr Number width = 0.125;
static constexpr Number wheel_diameter = 0.083;
static constexpr int ticks_per_revolution = 24;
static constexpr Number wheel_step = pi * wheel_diameter / ticks_per_revolution;

#endif
