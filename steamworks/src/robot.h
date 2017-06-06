#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
class OI;
namespace subsystem {
class Climber;
class Deadeye;
class SwerveDrive;
class GearLoader;
class Hopper;
class Intake;
class ShooterElevation;
class ShooterWheel;
} /* subsystem */

/** Robot is a command-based robot.
 * Hardware references are managed in robot_map and operator inputs in oi.
 */
class Robot : public frc::IterativeRobot {
 public:
  static OI* oi;
  static subsystem::Deadeye* deadeye;
  static subsystem::Climber* climber;
  static subsystem::SwerveDrive* drive;
  static subsystem::GearLoader* gear_loader;
  static subsystem::Hopper* hopper;
  static subsystem::Intake* intake;
  static subsystem::ShooterElevation* shooter_elevation;
  static subsystem::ShooterWheel* shooter_wheel;

  Robot();

  void RobotInit() override;
  void RobotPeriodic() override;

  void DisabledInit() override;
  void DisabledPeriodic() override;

  void AutonomousInit() override;
  void AutonomousPeriodic() override;

  void TeleopInit() override;
  void TeleopPeriodic() override;

  void TestInit() override;
  void TestPeriodic() override;

 private:
  std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<cpptoml::table> config_;
  frc::Command* autonomous_command_;
  frc::Command* stop_shooter_;
  double gyro_offset_ = 0;

  void Configure();
  void LogVersion();
};

} /* steamworks */
