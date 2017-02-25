#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <sidewinder/swerve/swerve_drive.h>
#include <sidewinder/talon/settings.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

class Motion;

class SwerveDrive : public sidewinder::swerve::SwerveDrive {
 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;

  void SetTeleOpMode();
  void SetAutonMode();

  void DriveAutonomous(double forward, double strafe, double azimuth);
  void DriveDistance(int distance);
  bool IsMotionDone();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<sidewinder::talon::Settings> drive_settings_;
  std::shared_ptr<sidewinder::talon::Settings> drive_auton_settings_;
  std::unique_ptr<Motion> motion_;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
