#pragma once

#include "AHRS.h"
#include "WPILib.h"
#include "swerve/swerve_drive.h"

namespace sidewinder {
class Drive : public frc::Subsystem {
 private:
  std::shared_ptr<AHRS> gyro_;
  swerve::SwerveDrive swerve_drive_;

 public:
  Drive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
  void ZeroWheels();
  void CartesianDrive(double forward, double strafe, double azimuth);
};
} /* sidewinder */
