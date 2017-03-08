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

  // SHOOTER SOLUTION CALCULATION
  void SetSolutionInputs(
      int centerline_elevation /*, int centerline_pixel_error*/);
  double GetSolutionRange();
  double GetSolutionElevation();
  double GetSolutionWheelSpeed();
  double GetSolutionAzimuthOffset();

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  // current setpoints for shooter motors
  int elevation_setpoint_ = 0;
  int speed_setpoint_ = 0;

  // shooter solution outputs
  double solution_elevation_;
  double solution_wheel_speed_;
  double solution_range_;
  double solution_azimuth_offset_;

  // shooter config settings
  int elevation_zero_ = 3550;          // encoder zero positon
  double elevation_zero_angle_ = 6.0;  // angle corresponding to encoder zero
  double centerline_height_ = 84.0;  // height of target centerline above ground
  double camera_height_ = 19.25;     // camera focal point distance from ground
  double camera_angle_ = 53.0;       // camera to shooter offset angle
  double degrees_per_tick_ = 0.00611;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void UpdateSmartDashboard();

  int LimitSpeed(int speed);
  int LimitElevation(int elevation);
};

} /* subsystem */
} /* steamworks */
