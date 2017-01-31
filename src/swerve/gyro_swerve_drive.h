#pragma once

#include "AHRS.h"
#include "cpptoml/cpptoml.h"
#include "swerve_drive.h"

namespace sidewinder {
namespace swerve {
class GyroSwerveDrive {
 private:
  SwerveDrive sd;
  AHRS ahrs;
  double inital_angle;
  bool first_time;

 public:
  GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                  const TalonMap* map);
  void ZeroAzimuth();
  void Drive(double forward, double strafe, double azimuth);
};
} /* swerve */
} /* sidewinder */
