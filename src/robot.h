#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "subsystems/climber.h"
#include "subsystems/drive.h"
#include "subsystems/gear_intake.h"
#include "subsystems/gear_loader.h"
#include "subsystems/hopper.h"
#include "subsystems/intake.h"
#include "subsystems/shooter.h"

namespace steamworks {

/** Robot is a command-based robot.
 * Hardware references are managed in robot_map and operator inputs in oi.
 */
class Robot : public frc::IterativeRobot {
 public:
  static OI* oi;
  static subsystem::Climber* climber;
  static subsystem::Drive* drive;
  static subsystem::GearIntake* gear_intake;
  static subsystem::GearLoader* gear_loader;
  static subsystem::Hopper* hopper;
  static subsystem::Intake* intake;
  static subsystem::Shooter* shooter;

  Robot();

  void RobotInit() override;

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

  void ConfigureLogging();
  void LoadConfig();
  void LogAbsoluteEncoders();
  void LogVersion();
};

} /* steamworks */
