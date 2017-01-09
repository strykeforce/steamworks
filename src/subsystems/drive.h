#pragma once

#include "WPILib.h"
#include "swerve/swerve_drive.h"

namespace steamworks {
class Drive : public frc::Subsystem {
 private:
  sidewinder::swerve::SwerveDrive swerve_drive_;

 public:
  Drive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
  void ZeroWheels();
  void CartesianDrive(float forward, float strafe, float azimuth);
};
} /* steamworks */
