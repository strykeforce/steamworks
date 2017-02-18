#include "shooter.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

/**
 * Shooter constructor.
 */
Shooter::Shooter(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Shooter"), logger_(spdlog::get("subsystem")) {
  // get talon parameters
  logger_->trace("starting constructor");
  auto steamworks_config = config->get_table("STEAMWORKS");
  elevation_settings_ =
      talon::Settings::Create(steamworks_config, "shooter_elevation");
  elevation_zero_settings_ =
      talon::Settings::Create(steamworks_config, "shooter_elevation_zero");

  auto speed_settings =
      talon::Settings::Create(steamworks_config, "shooter_wheel");

  // configure talons
  elevation_settings_->Initialize(RobotMap::shooter_elevation_talon);
  speed_settings->Initialize(RobotMap::shooter_wheel_talon);

  // logger_->debug("dumping shooter elevation talon configuration");
  // elevation_settings->LogConfig(logger_);
  // logger_->debug("dumping shooter wheel talon configuration");
  // speed_settings->LogConfig(logger_);
}

namespace {
const unsigned kElevationIncrement = 25;
const unsigned kSpeedIncrement = 10;

const unsigned kMinElevation = 1;
const unsigned kMaxElevation = 1600;

const unsigned kMinSpeed = 350;
const unsigned kMaxSpeed = 700;
}

/**
 * SetElevation sets the shooter elevation to the specified encoder position.
 */
void Shooter::SetElevation(unsigned elevation) {
  elevation_setpoint_ = LimitElevation(elevation);
  logger_->debug("setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(elevation);
}

/**
 * ElevationIncrement raises the elevation by a small constant amount.
 */
void Shooter::IncrementElevation() {
  elevation_setpoint_ += kElevationIncrement;
  logger_->debug("setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
}

/**
 * ElevationDecrement raises the elevation by a small constant amount.
 */
void Shooter::DecrementElevation() {
  elevation_setpoint_ -= kElevationIncrement;
  logger_->debug("setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
}

/**
 * GetElevationSetpoint returns the current elevation setpoint.
 * @returns elevation in encoder ticks
 */
unsigned Shooter::GetElevationSetpoint() { return elevation_setpoint_; }

/**
 * GetElevation returns the current elevation encoder position.
 * @returns elevation in encoder ticks
 */
unsigned Shooter::GetElevation() {
  auto position = RobotMap::shooter_elevation_talon->GetPosition();
  return static_cast<unsigned>(position);
}

/**
 * SetElevationZero drives the elevation against a known zero position.
 */
void Shooter::SetElevationZeroModeEnabled(bool enabled) {
  elevation_zero_mode_ = enabled;
  if (elevation_zero_mode_) {
    logger_->info("starting elevation zero mode");
    RobotMap::shooter_elevation_talon->StopMotor();
    elevation_zero_settings_->Initialize(RobotMap::shooter_elevation_talon);
    return;
  }
  logger_->info("finished elevation zero mode");
  RobotMap::shooter_elevation_talon->StopMotor();
  elevation_settings_->Initialize(RobotMap::shooter_elevation_talon);
}

/**
 * IsElevationZeroLimitSwitchClosed is true
 */
bool Shooter::IsElevationZeroLimitSwitchActive() {
  return !RobotMap::shooter_elevation_talon->IsRevLimitSwitchClosed();
}

/**
 * SetElevationZeroDriveVoltage drives the elevation against the zero stop.
 */
void Shooter::SetElevationZeroDriveVoltage(double volts) {
  if (!elevation_zero_mode_) {
    logger_->error("attempt to set voltage when not in elevation zero mode");
    return;
  }
  logger_->info("setting elevation zero mode drive voltage to {}", volts);
  RobotMap::shooter_elevation_talon->Set(volts);
}

/**
 * SetElevationEncoderZero sets the encoder for the elevation at known zero.
 */
void Shooter::SetElevationEncoderZero() {
  RobotMap::shooter_elevation_talon->SetPosition(0.0);
}

/**
 * SetSpeed sets the shooter wheel speed.
 */
void Shooter::SetSpeed(unsigned speed) {
  speed_setpoint_ = speed;
  logger_->debug("setting shooter wheel to {} ticks/100ms", speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
}

/**
 * SpeedIncrement raises the elevation by a small constant amount.
 */
void Shooter::IncrementSpeed() {
  speed_setpoint_ += kSpeedIncrement;
  // if (speed_setpoint_ > kMaxSpeed) {
  //   speed_setpoint_ = kMaxSpeed;
  //   logger_->warn("can't set wheel speed above {}", kMaxSpeed);
  // }
  logger_->debug("setting shooter wheel to {} ticks/100ms", speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
}

/**
 * SpeedDecrement raises the elevation by a small constant amount.
 */
void Shooter::DecrementSpeed() {
  speed_setpoint_ -= kSpeedIncrement;
  // if (speed_setpoint_ < kMinSpeed) {
  //   speed_setpoint_ = kMinSpeed;
  //   logger_->warn("can't set wheel speed below {}", kMinSpeed);
  // }
  logger_->debug("setting shooter wheel to {} ticks/100ms", speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
}

/**
 * GetSpeedSetpoint returns the current speed setpoint.
 * @returns speed in encoder ticks per 100 ms
 */
unsigned Shooter::GetSpeedSetpoint() const { return speed_setpoint_; }

/**
 * GetSpeed returns the current encoder speed.
 * @returns speed in encoder ticks per 100 ms
 */
unsigned Shooter::GetSpeed() const {
  auto speed = RobotMap::shooter_wheel_talon->GetSpeed();
  return static_cast<unsigned>(speed);
}

unsigned Shooter::LimitSpeed(unsigned speed) {
  if (speed < kMinSpeed) {
    logger_->warn("shooter wheel speed out of range: {}", speed);
    speed = kMinSpeed;
  }
  if (speed > kMaxSpeed) {
    logger_->warn("shooter wheel speed out of range: {}", speed);
    speed = kMaxSpeed;
  }
  return speed;
}

unsigned Shooter::LimitElevation(unsigned elevation) {
  if (elevation < kMinElevation) {
    logger_->warn("shooter elevation out of range: {}", elevation);
    elevation = kMinElevation;
  }
  if (elevation > kMaxElevation) {
    logger_->warn("shooter wheel elevation of range: {}", elevation);
    elevation = kMaxElevation;
  }
  return elevation;
}
