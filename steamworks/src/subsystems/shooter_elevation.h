#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

/**
 * ShooterElevation is the Boiler Fuel shooter subsystem.
 */
class ShooterElevation : public frc::Subsystem {
 public:
  ShooterElevation(const std::shared_ptr<cpptoml::table> config);
  ShooterElevation& operator=(ShooterElevation&) = delete;
  ShooterElevation(ShooterElevation&) = delete;

  // ELEVATION
  void SetElevation(int elevation);
  int GetElevationSetpoint();
  int GetElevation();

  // shooter testing
  void IncrementElevation();
  void DecrementElevation();

  // elevation encoder calibration
  void SetElevationEncoderZero();

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  // current setpoints for shooter motors
  int elevation_setpoint_ = 0;

  // shooter config settings
  int elevation_zero_ = 3550;  // encoder zero positon

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void UpdateSmartDashboard();

  int LimitElevation(int elevation);
};

} /* subsystem */
} /* steamworks */
