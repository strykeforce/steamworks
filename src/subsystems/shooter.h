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

  void SetElevation(int elevation);
  void IncrementElevation();
  void DecrementElevation();
  int GetElevationSetpoint();
  int GetElevation();

  void SetElevationEncoderZero();

  void SetSpeed(int speed);
  void IncrementSpeed();
  void DecrementSpeed();
  int GetSpeedSetpoint() const;
  int GetSpeed() const;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<sidewinder::talon::Settings> elevation_settings_;

  int elevation_setpoint_ = 0;
  int speed_setpoint_ = 0;

  int elevation_zero_ = 0;

  void UpdateSmartDashboard();

  int LimitSpeed(int speed);
  int LimitElevation(int elevation);
};

} /* subsystem */
} /* steamworks */
