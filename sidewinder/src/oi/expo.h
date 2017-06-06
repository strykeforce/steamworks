#pragma once

#include "cpptoml/cpptoml.h"

namespace sidewinder {
namespace oi {

/** Joystick deadzone elimination and sensitivity transfer function.
 * https://plot.ly/~jhh/11/matplotlib-notebook-import-matplotlibp/
 */
class Expo {
 private:
  double deadzone_, scale_, offset_;

 public:
  Expo(double deadzone, double scale);
  double operator()(double x) const;
};

} /* oi */
} /* sidewinder */
