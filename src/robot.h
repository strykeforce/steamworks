#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "subsystems/drive.h"
#include "subsystems/hopper.h"

namespace steamworks {

/** Robot is a command-based robot.
 * Hardware references are managed in robot_map and operator inputs in oi.
 */
class Robot : public frc::IterativeRobot {
 public:
  static OI* oi;
  static subsystem::Drive* drive;
  static subsystem::Hopper* hopper;

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
