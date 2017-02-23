#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/swerve/swerve_drive.h"
#include "sidewinder/talon/settings.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class SwerveDrive : public sidewinder::swerve::SwerveDrive {
 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;

  void SetTeleOpMode();
  void SetAutonMode();

  void DriveAutonomous(double forward, double strafe, double azimuth);

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<sidewinder::talon::Settings> drive_settings_;
  std::unique_ptr<sidewinder::talon::Settings> drive_auton_settings_;
};

} /* subsystem */
} /* steamworks */
