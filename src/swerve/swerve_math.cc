#include "swerve_math.h"

#include <iostream>

#include <algorithm>
#include <cmath>

using namespace sidewinder;

/** helper function for swerve drive calculations.
 *  https://www.chiefdelphi.com/media/papers/2426
 */
SwerveMath::SwerveMath() : last_dd_() {}

namespace {
const int L = 1;
const int W = 1;
const float R = std::sqrt(L * L + W * W);

void calc_wheels(DriveData& dd) {
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

inline std::pair<float, float> min_angle(float speed, float next, float prev) {
  // std::cout << "NEXT = " << next << " PREV = " << prev << std::endl;
  assert(speed >= 0);
  assert(next >= -180 && next <= 180);
  assert(prev >= -180 && prev <= 180);

  // alternate drive angle is 180 degrees from next ordered angle
  float alt = next + (next < 0 ? 180 : -180);

  // delta from each angle to last drive angle
  auto d_next = std::fabs(next - prev);
  auto d_alt = std::fabs(alt - prev);

  // use minumum angle change, reverse drive if alt angle used
  // prefer forward drive if both delta are equal
  if (d_next <= d_alt) {
    return std::pair<float, float>{speed, next};
  }
  // std::cout << "NEXT = " << next << " ALT = " << alt << std::endl;
  return std::pair<float, float>{-speed, alt};
}

} /* namespace */

/** Calculate drive wheel speeds and azimuth angles for all wheels. Inputs are
 * joystick forward, strafe and azimuth. Outputs are wheel speeds in the range
 * 0 to +1 and wheel angles in the range -180 to 180 degrees.
 */
void SwerveMath::Calc(DriveData& dd) {
  calc_wheels(dd);

  auto wheel = min_angle(dd.wsrf, dd.warf, last_dd_.warf);
  dd.wsrf = wheel.first;
  dd.warf = wheel.second;

  wheel = min_angle(dd.wslf, dd.walf, last_dd_.walf);
  dd.wslf = wheel.first;
  dd.walf = wheel.second;

  wheel = min_angle(dd.wslr, dd.walr, last_dd_.walr);
  dd.wslr = wheel.first;
  dd.walr = wheel.second;

  wheel = min_angle(dd.wsrr, dd.warr, last_dd_.warr);
  dd.wsrr = wheel.first;
  dd.warr = wheel.second;

  last_dd_ = dd;
}
