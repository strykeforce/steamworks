#pragma once

#include "WPILib.h"
#include "sidewinder/swerve/gyro_swerve_drive.h"

namespace steamworks {
namespace subsystem {

class Drive : public frc::Subsystem {
 private:
  sidewinder::swerve::GyroSwerveDrive swerve_drive_;

 public:
  Drive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
  void ZeroWheels();
  void CartesianDrive(float forward, float strafe, float azimuth);
};

} /* subsystem */
} /* steamworks */
