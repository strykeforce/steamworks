#include "swerve_math.h"

#include <spdlog/spdlog.h>
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

Vector2f* calc_rotation_vectors(double robot_left_to_right_length,
                                double robot_front_to_back_length,
                                Vector2f rotation_cord) {
  Vector2f cord_multiplers[4];
  cord_multiplers[WheelPoints::RightFront] = {+1, +1};
  cord_multiplers[WheelPoints::RightRear] = {+1, -1};
  cord_multiplers[WheelPoints::LeftRear] = {-1, -1};
  cord_multiplers[WheelPoints::LeftFront] = {-1, +1};
  Vector2f* to_be_returned = (Vector2f*)malloc(sizeof(Vector2f) * 4);
  unsigned int i;
  for (i = 0; i < 4; i++) {
    double X =
        rotation_cord.x - robot_left_to_right_length * cord_multiplers[i].x / 2;
    double Y =
        rotation_cord.y - robot_front_to_back_length * cord_multiplers[i].y / 2;
    double distance = hypot_(X, Y);
    to_be_returned[i].x = -Y / distance;
    to_be_returned[i].y = X / distance;
  }
  return to_be_returned;
}

SwerveMath::SwerveMath(const std::shared_ptr<cpptoml::table> config) {
  LoadConfigSettings(config);
}

/** Calculate drive wheel speeds and azimuth angles for all wheels. Inputs are
 * joystick forward, strafe and azimuth. Outputs are wheel speeds in the range
 * 0 to +1 and wheel angles are in degrees, with no range limit
 */

void SwerveMath::operator()(DriveData& dd, enum RotationPoint rp) {
  struct Vector2f* current_rotation_vectors = rotation_vectors[rp];
  for (int i = 0; i < 4; i++) {
    // just adding vectors together
    double x = dd.str + current_rotation_vectors[i].x * dd.rcw;
    double y = dd.fwd + current_rotation_vectors[i].y * dd.rcw;
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
#if 1
    if (smart_angle_change > 90) {
      wheel_angles[i] = wheel_angles[i] - 180;
      wheel_mag_negated[i] = !wheel_mag_negated[i];
    } else if (smart_angle_change < -90) {
      wheel_angles[i] = wheel_angles[i] + 180;
      wheel_mag_negated[i] = !wheel_mag_negated[i];
    }
#endif
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

/**
 * Override the wheel angle history. Uses DriveData members warf, walf, walr,
 * warr only.
 */
void SwerveMath::SetWheelAngles(const DriveData& dd) {
  wheel_anglesPast[0] = fmod(dd.warf + 360, 360);
  wheel_anglesPast[1] = fmod(dd.walf + 360, 360);
  wheel_anglesPast[2] = fmod(dd.walr + 360, 360);
  wheel_anglesPast[3] = fmod(dd.warr + 360, 360);
}

/**
 * Load configuration settings
 */
void SwerveMath::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config) {
  auto settings = config->get_table("SIDEWINDER");
  if (!settings) {
    throw std::invalid_argument("SIDEWINDER config is missing");
  }

  settings = settings->get_table("SWERVE");
  if (!settings) {
    throw std::invalid_argument("SWERVE config is missing");
  }

  auto wb_w = settings->get_as<double>("wheelbase_width");
  if (!wb_w) {
    throw std::invalid_argument(
        "SIDEWINDER wheelbase_width setting is missing");
  }
  double wheelbase_width = *wb_w;

  auto wb_l = settings->get_as<double>("wheelbase_length");
  if (!wb_l) {
    throw std::invalid_argument(
        "SIDEWINDER wheelbase_length setting is missing");
  }
  double wheelbase_length = *wb_l;

  auto srx = settings->get_as<double>("shooter_rotation_point_X");
  if (!srx) {
    throw std::invalid_argument(
        "SIDEWINDER shooter_rotation_point_X setting is missing");
  }
  double srx_value = *srx;

  auto sry = settings->get_as<double>("shooter_rotation_point_Y");
  if (!sry) {
    throw std::invalid_argument(
        "SIDEWINDER shooter_rotation_point_Y setting is missing");
  }
  double sry_value = *sry;

  auto logger = spdlog::get("sidewinder");
  logger->info("wheelbase W = {}, L = {}", wheelbase_width, wheelbase_length);

  rotation_vectors[RotationPoint::Center] =
      calc_rotation_vectors(wheelbase_width, wheelbase_length, {0.0, 0.0});
  rotation_vectors[RotationPoint::Shooter] = calc_rotation_vectors(
      wheelbase_width, wheelbase_length, {srx_value, sry_value});
}
