#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "swerve_math.h"
#include "talon_map.h"

namespace sidewinder {
namespace swerve {

/** SwerveDrive is a WPI command-based subsystem represents the Sidewinder
 * swerve drive.
 */
class SwerveDrive {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const TalonMap* map_;
  SwerveMath swerve_math_;
  double drive_scale_factor_ = 0.0;

  void SetEncoderZero(const std::shared_ptr<cpptoml::table> config);

 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config,
              const TalonMap* map);
  void ZeroAzimuth();
  void Drive(double forward, double strafe, double azimuth);
  void CrabDrive(double forward, double strafe);
};
} /* swerve */
} /* sidewinder */
