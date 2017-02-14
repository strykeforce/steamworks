#pragma once

#include "AHRS.h"
#include "cpptoml/cpptoml.h"
#include "swerve_drive.h"

namespace sidewinder {
namespace swerve {
class GyroSwerveDrive {
 private:
  SwerveDrive sd_;
  std::shared_ptr<AHRS> ahrs_;
  double initial_angle_;
  bool first_time_;

 public:
  GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                  const TalonMap* map);

  GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                  const TalonMap* map, std::shared_ptr<AHRS> gyro);

  void ZeroAzimuth();

  void Drive(double forward, double strafe, double azimuth);
};
} /* swerve */
} /* sidewinder */
