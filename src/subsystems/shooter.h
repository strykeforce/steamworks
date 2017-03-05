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

  void SetElevation(unsigned elevation);
  void IncrementElevation();
  void DecrementElevation();
  unsigned GetElevationSetpoint();
  unsigned GetElevation();

  void SetElevationZeroModeEnabled(bool enabled);
  bool IsElevationZeroLimitSwitchActive();
  void SetElevationZeroDriveVoltage(double volts);
  void SetElevationEncoderZero();

  void SetSpeed(unsigned speed);
  void IncrementSpeed();
  void DecrementSpeed();
  unsigned GetSpeedSetpoint() const;
  unsigned GetSpeed() const;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<sidewinder::talon::Settings> elevation_settings_;
  std::unique_ptr<sidewinder::talon::Settings> elevation_zero_settings_;

  unsigned elevation_setpoint_ = 0;
  unsigned speed_setpoint_ = 0;

  bool elevation_zero_mode_ = false;

  void UpdateSmartDashboard();

  unsigned LimitSpeed(unsigned speed);
  unsigned LimitElevation(unsigned elevation);
};

} /* subsystem */
} /* steamworks */
