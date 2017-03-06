#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/talon/settings.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

/**
 * Shooter is the Boiler Fuel shooter subsystem.
 */
class Shooter : public frc::Subsystem {
 public:
  Shooter(const std::shared_ptr<cpptoml::table> config);
  Shooter& operator=(Shooter&) = delete;
  Shooter(Shooter&) = delete;

  // ELEVATION
  void SetElevation(int elevation);
  int GetElevationSetpoint();
  int GetElevation();
  // shooter testing
  void IncrementElevation();
  void DecrementElevation();
  // elevation encoder calibration
  void SetElevationEncoderZero();

  // WHEEL SPEED
  void SetSpeed(int speed);
  int GetSpeedSetpoint() const;
  int GetSpeed() const;
  // shooter testing
  void IncrementSpeed();
  void DecrementSpeed();

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  int elevation_setpoint_ = 0;
  int speed_setpoint_ = 0;

  int elevation_zero_ = 0;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void UpdateSmartDashboard();

  int LimitSpeed(int speed);
  int LimitElevation(int elevation);
};

} /* subsystem */
} /* steamworks */
