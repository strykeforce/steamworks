#include "swerve_math.h"

#include <cmath>
#include <cstring>

constexpr double PI = 3.14159265358979323846;
constexpr double TAU = 2.0 * PI;

inline int basic_compare(double a, double b) {
  return ((a == b) ? 0 : ((a > b) ? 1 : -1));
}
inline int sign(double val) { return basic_compare(val, 0); }
inline double hypot_(double x, double y) { return std::sqrt(x * x + y * y); }
inline double to_degrees(double rad) { return (rad * 360.0 / TAU); }

using namespace sidewinder::swerve;

SwerveMath::SwerveMath() {}

namespace {
constexpr double L = 1.0;
constexpr double W = 1.0;
const double robot_diagonal = hypot_(L, W);
const double hd = L / robot_diagonal;
const double wd = W / robot_diagonal;
const double rotation_vectors[4][2] = {
    {-hd, -wd},  // [0]
    {-hd, wd},   // [1]
    {hd, wd},    // [2]
    {hd, -wd},   // [3]
};

} /* namespace */

/** Calculate drive wheel speeds and azimuth angles for all wheels. Inputs are
 * joystick forward, strafe and azimuth. Outputs are wheel speeds in the range
 * 0 to +1 and wheel angles are in degrees, with no range limit
 */

void SwerveMath::operator()(DriveData& dd) {
  for (int i = 0; i < 4; i++) {
    // just adding vectors together
    double x = dd.str + rotation_vectors[i][0] * dd.rcw;
    double y = dd.fwd + rotation_vectors[i][1] * dd.rcw;
    double wheel_angle = to_degrees(atan2(y, x));
    // finding best angle
    wheel_angle = fmod(wheel_angle + 270, 360);
    double raw_angle_change = wheel_angle - wheel_anglesPast[i];
    double option1 = raw_angle_change;
    double option2 = 360.0 - fabs(raw_angle_change);
    option2 = -sign(raw_angle_change) * option2;
    // other best angle
    // for the record, integer 'add()' would work just the same:
    double smart_angle_change =
        (fabs(option1) < fabs(option2)) ? option1 : option2;
    wheel_anglesPast[i] = wheel_anglesPast[i] + smart_angle_change;
    wheel_anglesPast[i] = fmod(wheel_anglesPast[i] + 360, 360);
    wheel_angles[i] = wheel_angles[i] + smart_angle_change;
    if (smart_angle_change > 90) {
      wheel_angles[i] = wheel_angles[i] - 180;
      wheel_mag_negated[i] = !wheel_mag_negated[i];
    } else if (smart_angle_change < -90) {
      wheel_angles[i] = wheel_angles[i] + 180;
      wheel_mag_negated[i] = !wheel_mag_negated[i];
    }
    // wheel speed
    wheel_mags[i] = hypot_(x, y);
  }

  double max_wheel_mag = 0;
  for (int i = 0; i < 4; i++) {
    if (max_wheel_mag < wheel_mags[i]) {
      max_wheel_mag = wheel_mags[i];
    }
    if (wheel_mag_negated[i]) {
      wheel_mags[i] *= -1;
    }
  }

  // if desired speed is larger than 100%, cut speed
  if (max_wheel_mag > 1) {
    for (int i = 0; i < 4; i++) {
      wheel_mags[i] /= max_wheel_mag;
    }
  }

  dd.warf = wheel_angles[0];
  dd.wsrf = wheel_mags[0];
  dd.walf = wheel_angles[1];
  dd.wslf = wheel_mags[1];
  dd.walr = wheel_angles[2];
  dd.wslr = wheel_mags[2];
  dd.warr = wheel_angles[3];
  dd.wsrr = wheel_mags[3];
}
