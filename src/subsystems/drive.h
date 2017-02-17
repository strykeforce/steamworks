#pragma once

#include "AHRS.h"
#include "WPILib.h"
#include "sidewinder/swerve/swerve_drive.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Drive : public frc::Subsystem {
 public:
  Drive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
  void ZeroWheels();
  void CartesianDrive(float forward, float strafe, float azimuth);
  int GetPosition(sidewinder::swerve::SwerveDrive::Wheel wheel =
                      sidewinder::swerve::SwerveDrive::kRightRear) const;
  int GetAzimuth(sidewinder::swerve::SwerveDrive::Wheel wheel =
                     sidewinder::swerve::SwerveDrive::kRightRear) const;

  void WriteAzimuthCalibration();
  void ReadAzimuthCalibration();

  float GetGyroYaw();
  void ZeroGyroYaw();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<AHRS> gyro_;
  sidewinder::swerve::SwerveDrive swerve_drive_;
};

} /* subsystem */
} /* steamworks */
