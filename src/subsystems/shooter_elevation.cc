#include "shooter_elevation.h"

#include <sidewinder/talon/settings.h>
#include <cmath>

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

// TODO: put these in config
namespace {
const int kElevationIncrement = 25;
const int kMinElevation = 0;
const int kMaxElevation = 5000;
}

/**
 * ShooterElevation constructor.
 */
ShooterElevation::ShooterElevation(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("ShooterElevation"), logger_(spdlog::get("subsystem")) {
  LoadConfigSettings(config);
  SetElevationEncoderZero();
}

/**
 * SetElevation sets the shooter elevation to the specified encoder position.
 */
void ShooterElevation::SetElevation(int elevation) {
  elevation_setpoint_ = LimitElevation(elevation);
  // SPDLOG_DEBUG(logger_, "setting shooter elevation to {}",
  // elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
  UpdateSmartDashboard();
}

/**
 * ElevationIncrement raises the elevation by a small constant amount.
 */
void ShooterElevation::IncrementElevation() {
  elevation_setpoint_ += kElevationIncrement;
  elevation_setpoint_ = LimitElevation(elevation_setpoint_);
  SPDLOG_DEBUG(logger_, "setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
  UpdateSmartDashboard();
}

/**
 * ElevationDecrement raises the elevation by a small constant amount.
 */
void ShooterElevation::DecrementElevation() {
  elevation_setpoint_ -= kElevationIncrement;
  elevation_setpoint_ = LimitElevation(elevation_setpoint_);
  SPDLOG_DEBUG(logger_, "setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
  UpdateSmartDashboard();
}

/**
 * GetElevationSetpoint returns the current elevation setpoint.
 * @returns elevation in encoder ticks
 */
int ShooterElevation::GetElevationSetpoint() { return elevation_setpoint_; }

/**
 * GetElevation returns the current elevation encoder position.
 * @returns elevation in encoder ticks
 */
int ShooterElevation::GetElevation() {
  auto position = RobotMap::shooter_elevation_talon->GetPosition();
  return static_cast<int>(position);
}

/**
 * SetElevationEncoderZero sets the encoder for the elevation at known zero.
 */
void ShooterElevation::SetElevationEncoderZero() {
  auto pos = RobotMap::shooter_elevation_talon->GetPulseWidthPosition() & 0xFFF;
  double error = static_cast<double>(elevation_zero_ - pos);
  RobotMap::shooter_elevation_talon->SetPosition(error);
  logger_->info(
      "ShooterElevation position {}, configured zero {}, set position {}", pos,
      elevation_zero_, error);
}

namespace {
const std::string kWheelElevationKey("Shooter Elevation");
}

/**
 * Send elevation and speed to SmartDashboard.
 */
void ShooterElevation::UpdateSmartDashboard() {
#if !NDEBUG
  SmartDashboard::PutNumber(kWheelElevationKey, elevation_setpoint_);
#endif
}

/**
 * Check shooter elevation against limits.
 */
int ShooterElevation::LimitElevation(int elevation) {
  if (elevation < kMinElevation) {
    logger_->warn("ShooterElevation out of range {}, setting to {}", elevation,
                  kMinElevation);
    return kMinElevation;
  }
  if (elevation > kMaxElevation) {
    logger_->warn("ShooterElevation out of range {}, setting to {}", elevation,
                  kMaxElevation);
    return kMaxElevation;
  }
  return elevation;
}

void ShooterElevation::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("SHOOTER");
  if (!config) {
    throw std::invalid_argument("STEAMWORKS.SHOOTER table missing from config");
  }

  auto i_opt = config->get_as<int>("elevation_zero");
  if (i_opt) {
    elevation_zero_ = *i_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER elevation_zero setting not available, using "
        "default");
  }
  logger_->info("shooter elevation zero position: {}", elevation_zero_);

  // configure talons
  auto elevation_settings =
      talon::Settings::Create(config, "shooter_elevation");
  elevation_settings->Initialize(RobotMap::shooter_elevation_talon);
  logger_->info("shooter elevation Talon initialized");
}
