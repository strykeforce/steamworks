#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

/**
 * ShooterWheel is the Boiler Fuel shooter subsystem.
 */
class ShooterWheel : public frc::Subsystem {
 public:
  ShooterWheel(const std::shared_ptr<cpptoml::table> config);
  ShooterWheel& operator=(ShooterWheel&) = delete;
  ShooterWheel(ShooterWheel&) = delete;

  // WHEEL SPEED
  void SetSpeed(int speed);
  int GetSpeedSetpoint() const;
  int GetSpeed() const;

  // shooter testing
  void IncrementSpeed();
  void DecrementSpeed();

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  // current setpoints for shooter motors
  int speed_setpoint_ = 0;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void UpdateSmartDashboard();

  int LimitSpeed(int speed);
};

} /* subsystem */
} /* steamworks */
