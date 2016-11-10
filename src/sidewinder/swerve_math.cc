#include "swerve_math.h"

#include <iostream>
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

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
  dd.wsrf = std::sqrt(b * b + c * c);
  dd.wslf = std::sqrt(b * b + d * d);
  dd.wslr = std::sqrt(a * a + d * d);
  dd.wsrr = std::sqrt(a * a + c * c);
  dd.warf = std::atan2(b, c) * 180 / M_PI;
  dd.walf = std::atan2(b, d) * 180 / M_PI;
  dd.walr = std::atan2(a, d) * 180 / M_PI;
  dd.warr = std::atan2(a, c) * 180 / M_PI;

  // normalize wheel speed if needed
  float max = std::max({dd.wsrf, dd.wslf, dd.wslr, dd.wsrr});
  if (max > 1.0) {
    dd.wsrf /= max;
    dd.wslf /= max;
    dd.wslr /= max;
    dd.wsrr /= max;
  }
}

} /* sidewinder */
