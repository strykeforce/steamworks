#pragma once

#include "AHRS.h"
#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "swerve_math.h"
#include "talon_map.h"

namespace sidewinder {
namespace talon {
class Settings;
}
namespace swerve {

/** SwerveDrive is a WPI command-based subsystem represents the Sidewinder
 * swerve drive.
 */
class SwerveDrive : public frc::Subsystem {
 public:
  /** Swerve drive wheels. */
  enum Wheel {
    kRightFront,
    kLeftFront,
    kLeftRear,
    kRightRear,
  };

  SwerveDrive(const std::string& name,
              const std::shared_ptr<cpptoml::table> config, const TalonMap* map,
              std::shared_ptr<AHRS> gyro);

  SwerveDrive(const std::shared_ptr<cpptoml::table> config, const TalonMap* map,
              std::shared_ptr<AHRS> gyro);

  virtual ~SwerveDrive() = default;

  // normal driving methods
  void Drive(double forward, double strafe, double azimuth);

  void TargetRotation(double azimuth);

  int GetAzimuth(const Wheel wheel = kRightRear) const;
  int GetPosition(const Wheel wheel = kRightRear) const;

  // special driving or troubleshooting methods
  void SetAzimuth(double setpoint);  // set all to same setpoint
  void SetDrive(double setpoint);    // set all to same setpoint
  void ZeroAzimuth();

  void SetGyroDisabled(bool disabled);
  void SetDriveMode(const std::shared_ptr<talon::Settings> settings,
                    double setpoint_scale);

  void WriteAzimuthCalibration();
  void ReadAzimuthCalibration();

  void CrabDriveAutonomous(double setpoint, int azimuth);

  // utility methods
  void SetLogger(const std::shared_ptr<spdlog::logger> logger);

 protected:
  std::shared_ptr<spdlog::logger> logger_;
  double drive_scale_factor_ = 0.0;
  const TalonMap* map_;

  void Drive_(double forward, double strafe, double azimuth, double dead_zone);

 private:
  std::shared_ptr<AHRS> ahrs_;
  SwerveMath swerve_math_;
  bool gyro_disabled_ = false;
  double dead_zone_;
};
} /* swerve */
} /* sidewinder */
