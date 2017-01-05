#include "expo.h"

using namespace sidewinder::oi;

/** Initialize the expo transfer function.
 */
Expo::Expo(double deadzone, double scale) : deadzone_(deadzone), scale_(scale) {
  if (deadzone_ < 0.0 || deadzone_ >= 1.0) {
    throw std::invalid_argument("deadzone out of range");
  }

  if (scale_ < 0.0 || scale_ > 1.0) {
    throw std::invalid_argument("scale out of range");
  }

  offset_ = 1.0 / (scale_ * std::pow(1.0 - deadzone_, 3) +
                   (1.0 - scale_) * (1.0 - deadzone_));
}

/** Enforce deadzone and perform exponential input scaling.
 */
double Expo::operator()(double x) const {
  if (std::fabs(x) < deadzone_) {
    return 0.0;
  }
  x = x > 0.0 ? x - deadzone_ : x + deadzone_;
  return (scale_ * std::pow(x, 3) + (1 - scale_) * x) * offset_;
}
