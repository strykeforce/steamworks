#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "talon_map.h"

namespace sidewinder {

/** SwerveDrive is a WPI command-based subsystem represents the Sidewinder
 * swerve drive.
 */
class SwerveDrive : public ::Subsystem {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const TalonMap* map_;
  const OI* oi_;
  float max_voltage_ = 12.0;  // voltage limit in voltage mode

  void SetEncoderZero(const std::shared_ptr<cpptoml::table> config);

 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config, const TalonMap* map,
              const OI* oi);
  void InitDefaultCommand() override;
  void ZeroAzimuth();
  void Drive(float forward, float strafe, float azimuth);
  void CrabDrive(float forward, float strafe);
};
} /* sidewinder */
