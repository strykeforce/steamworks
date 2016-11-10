#include "swerve_math.h"

#include <algorithm>
#include <cmath>

namespace sidewinder {

SwerveMath::SwerveMath() {}

// helper function for swerve drive calculations.
// https://www.chiefdelphi.com/media/papers/2426
namespace {
const int L = 1;
const int W = 1;
const float R = std::sqrt(L * L + W * W);

} /* namespace */

void SwerveMath::Calc(DriveData& dd) {
  float a = dd.str - dd.rcw * (L / R);
  float b = dd.str + dd.rcw * (L / R);
  float c = dd.fwd - dd.rcw * (W / R);
  float d = dd.fwd + dd.rcw * (W / R);
  dd.ws1 = std::sqrt(b * b + c * c);
  dd.ws2 = std::sqrt(b * b + d * d);
  dd.ws3 = std::sqrt(a * a + d * d);
  dd.ws4 = std::sqrt(a * a + c * c);
  dd.wa1 = std::atan2(b, c) * 180 / M_PI;
  dd.wa2 = std::atan2(b, d) * 180 / M_PI;
  dd.wa3 = std::atan2(a, d) * 180 / M_PI;
  dd.wa4 = std::atan2(a, c) * 180 / M_PI;

  // normalize wheel speed if needed
  float max = std::max({dd.ws1, dd.ws2, dd.ws3, dd.ws4});
  if (max > 1.0) {
    dd.ws1 /= max;
    dd.ws2 /= max;
    dd.ws3 /= max;
    dd.ws4 /= max;
  }
}

} /* sidewinder */
