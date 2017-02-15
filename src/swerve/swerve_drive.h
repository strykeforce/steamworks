#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "swerve_math.h"
#include "talon_map.h"

namespace sidewinder {
namespace swerve {

/** SwerveDrive is a WPI command-based subsystem represents the Sidewinder
 * swerve drive.
 */

class SwerveDrive {
 public:
  /** Swerve drive wheels. */
  enum Wheel {
    kRightFront,
    kLeftFront,
    kLeftRear,
    kRightRear,
  };

  SwerveDrive(const std::shared_ptr<cpptoml::table> config,
              const TalonMap* map);
  // normal driving methods
  void Drive(double forward, double strafe, double azimuth);
  void TargetRotation(double azimuth);
  int GetAzimuth(const Wheel wheel = kRightRear) const;
  int GetPosition(const Wheel wheel = kRightRear) const;

  // special driving or troubleshooting methods
  void CrabDrive(double speed, double direction);
  void ZeroAzimuth();
  void WriteAzimuthCalibration();
  void ReadAzimuthCalibration();

  // utility methods
  void SetLogger(const std::shared_ptr<spdlog::logger> logger);

 private:
  std::shared_ptr<spdlog::logger> logger_;
  const TalonMap* map_;
  SwerveMath swerve_math_;
  double drive_scale_factor_ = 0.0;
};
} /* swerve */
} /* sidewinder */
