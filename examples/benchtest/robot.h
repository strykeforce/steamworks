#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

// #include "oi.h"
#include "sidewinder/swerve_drive.h"

namespace sidewinder {

class Robot : public ::IterativeRobot {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<cpptoml::table> config_;

  void LoadConfig();
  void LogAbsoluteEncoders();

 public:
  // static const OI* oi;
  static SwerveDrive* swerve_drive;

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
};

} /* sidewinder */
