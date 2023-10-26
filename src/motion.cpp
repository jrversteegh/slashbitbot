#include "motors.h"
#include "sensors.h"
#include "motion.h"

static constexpr Number pi = 3.14159265358979323846;
static constexpr Number width = 0.125;
static constexpr Number wheel_step = pi * 0.083 / 24;

static Locor actual_locor{};
static Locor target_locor{};

void initialize_motion() {
}

Locor get_locor() {
  return actual_locor;
}

void move(Vector const& v) {
  target_locor = actual_locor;
  target_locor += v;
}

void move(RelativeVector const& rv) {
  target_locor = actual_locor;
  target_locor += rv;
}
