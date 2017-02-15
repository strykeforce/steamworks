#pragma once

#include "AHRS.h"
#include "cpptoml/cpptoml.h"
#include "swerve_drive.h"

namespace sidewinder {
namespace swerve {
class GyroSwerveDrive {
 public:
  GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                  const TalonMap* map);

  GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                  const TalonMap* map, std::shared_ptr<AHRS> gyro);

  // normal driving methods
  void Drive(double forward, double strafe, double azimuth);
  void TargetRotation(double azimuth);
  int GetPosition(
      const SwerveDrive::Wheel wheel = SwerveDrive::kRightRear) const;
  int GetAzimuth(
      const SwerveDrive::Wheel wheel = SwerveDrive::kRightRear) const;

  // special driving or troubleshooting methods
  void ZeroAzimuth();
  void SetGyroDisabled(bool disabled);
  void WriteAzimuthCalibration();
  void ReadAzimuthCalibration();

  // utility methods
  void SetLogger(const std::shared_ptr<spdlog::logger> logger);

 private:
  std::shared_ptr<spdlog::logger> logger_;
  SwerveDrive sd_;
  std::shared_ptr<AHRS> ahrs_;
  double initial_angle_;
  bool gyro_disabled_;
};
} /* swerve */
} /* sidewinder */
