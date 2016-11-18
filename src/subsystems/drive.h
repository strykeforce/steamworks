#pragma once

#include "WPILib.h"
#include "swerve/swerve_drive.h"

namespace avenger {
class Drive : public ::Subsystem {
 private:
  sidewinder::SwerveDrive swerve_drive_;

 public:
  Drive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
  void ZeroWheels();
  void CartesianDrive(float forward, float strafe, float azimuth);
};
} /* avenger */
