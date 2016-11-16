#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "swerve_math.h"
#include "talon_map.h"

namespace sidewinder {

/** SwerveDrive is a WPI command-based subsystem represents the Sidewinder
 * swerve drive.
 */
class SwerveDrive {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const TalonMap* map_;
  SwerveMath swerve_math_;
  float max_voltage_ = 12.0;  // voltage limit in voltage mode

  void SetEncoderZero(const std::shared_ptr<cpptoml::table> config);

 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config,
              const TalonMap* map);
  void ZeroAzimuth();
  void Drive(float forward, float strafe, float azimuth);
  void CrabDrive(float forward, float strafe);
};
} /* sidewinder */
