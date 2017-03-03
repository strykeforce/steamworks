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
  void SetAzimuthMode();
  void SetMotionMagicMode();

  // these three methods work together for autonomous azimuth
  void PositionAzimuthForAuton();  // note: valid for 2048 starting pos
  bool IsPositionAzimuthForAutonDone();
  void DriveAzimuthAutonomous(double setpoint);

  void DriveAutonomous(double forward, double strafe, double azimuth);

  void ZeroDistance();
  void DriveDistance(int distance, int azimuth);
  bool IsMotionDone();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<sidewinder::talon::Settings> drive_settings_;
  std::shared_ptr<sidewinder::talon::Settings> drive_auton_settings_;
  std::shared_ptr<sidewinder::talon::Settings> drive_azimuth_settings_;
  std::shared_ptr<sidewinder::talon::Settings> drive_motion_magic_settings_;
  double distance_;
  // std::unique_ptr<Motion> motion_;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
