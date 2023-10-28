#ifndef SLASHBITBOT_FUNCTIONS_H__
#define SLASHBITBOT_FUNCTIONS_H__

#include <cmath>

using Number = float;

inline Number angle_mod(Number const angle) {
  Number result = std::fmod(angle, 360.0);
  return result < 0.0 ? result + 360.0 : result;
}

inline Number angle_mod_signed(Number const angle) {
  Number result = std::fmod(angle, 360.0);
  return result < -180.0 ? result + 360.0 : result >= 180.0 ? result - 360.0 : result;
}

inline Number angle_diff(Number const angle1, Number const angle2) {
  return angle_mod_signed(angle1 - angle2);
}

#endif
