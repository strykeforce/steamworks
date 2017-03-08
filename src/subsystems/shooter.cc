#include "shooter.h"

#include <cmath>

#include "robot_map.h"
#include "shooter_target_data.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

// TODO: put these in config
namespace {
const int kElevationIncrement = 25;
const int kSpeedIncrement = 10;

const int kMinElevation = 0;
const int kMaxElevation = 6000;

const int kMinSpeed = 350;
const int kMaxSpeed = 700;
}

/**
 * Shooter constructor.
 */
Shooter::Shooter(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Shooter"), logger_(spdlog::get("subsystem")) {
  LoadConfigSettings(config);
  SetElevationEncoderZero();
}

/**
 * Supply shooter solution input data.
 */
void Shooter::SetSolutionInputs(int centerline_elevation /*,
                                int centerline_pixel_error*/) {
  double shooter_angle =
      elevation_zero_angle_ + (centerline_elevation * degrees_per_tick_);
  logger_->debug("shooter angle = {}", shooter_angle);

  double aiming_angle = ((camera_angle_ - shooter_angle) / 360.0) * 2.0 * M_PI;
  logger_->debug("aiming angle = {}", aiming_angle);

  solution_range_ =
      (centerline_height_ - camera_height_) / std::tan(aiming_angle);

  solution_range_ += 10;  // front of target to center of boiler
  logger_->debug("solution range = {}", solution_range_);

  int range_lookup =
      static_cast<int>(std::round(solution_range_)) - shooter_range_offset;
  if (range_lookup < 0 || range_lookup > shooter_data_size) {
    logger_->error("range lookup value out of range, solution range = {}",
                   solution_range_);
    return;
  }
  solution_elevation_ = shooter_data[range_lookup][kElevation];
  solution_wheel_speed_ = shooter_data[range_lookup][kSpeed];
  solution_azimuth_offset_ = shooter_data[range_lookup][kAzimuth];

  logger_->info(
      "Shooter solution: range = {}, elevation = {}, speed = {}, azimuth "
      "offset = {}",
      solution_range_, solution_elevation_, solution_wheel_speed_,
      solution_azimuth_offset_);
}

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Shooter::GetSolutionRange() { return solution_range_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Shooter::GetSolutionElevation() { return solution_elevation_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Shooter::GetSolutionWheelSpeed() { return solution_wheel_speed_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Shooter::GetSolutionAzimuthOffset() { return solution_azimuth_offset_; }

/**
 * SetElevation sets the shooter elevation to the specified encoder position.
 */
void Shooter::SetElevation(int elevation) {
  elevation_setpoint_ = LimitElevation(elevation);
  SPDLOG_DEBUG(logger_, "setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(elevation);
  UpdateSmartDashboard();
}

/**
 * ElevationIncrement raises the elevation by a small constant amount.
 */
void Shooter::IncrementElevation() {
  elevation_setpoint_ += kElevationIncrement;
  SPDLOG_DEBUG(logger_, "setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
  UpdateSmartDashboard();
}

/**
 * ElevationDecrement raises the elevation by a small constant amount.
 */
void Shooter::DecrementElevation() {
  elevation_setpoint_ -= kElevationIncrement;
  SPDLOG_DEBUG(logger_, "setting shooter elevation to {}", elevation_setpoint_);
  RobotMap::shooter_elevation_talon->Set(
      static_cast<double>(elevation_setpoint_));
  UpdateSmartDashboard();
}

/**
 * GetElevationSetpoint returns the current elevation setpoint.
 * @returns elevation in encoder ticks
 */
int Shooter::GetElevationSetpoint() { return elevation_setpoint_; }

/**
 * GetElevation returns the current elevation encoder position.
 * @returns elevation in encoder ticks
 */
int Shooter::GetElevation() {
  auto position = RobotMap::shooter_elevation_talon->GetPosition();
  return static_cast<int>(position);
}

/**
 * SetElevationEncoderZero sets the encoder for the elevation at known zero.
 */
void Shooter::SetElevationEncoderZero() {
  auto pos = RobotMap::shooter_elevation_talon->GetPulseWidthPosition() & 0xFFF;
  SPDLOG_DEBUG(logger_, "elevation_talon absolute encoder = {}", pos);
  int error = elevation_zero_ - pos;
  error += pos > elevation_zero_ ? 0xFFF : 0;
  SPDLOG_DEBUG(logger_, "elevation_talon error = {}", error);
  SPDLOG_DEBUG(logger_, "setting elevation_talon zero = {}", error);
  RobotMap::shooter_elevation_talon->SetPosition(error);
}

/**
 * SetSpeed sets the shooter wheel speed.
 */
void Shooter::SetSpeed(int speed) {
  speed_setpoint_ = speed;
  SPDLOG_DEBUG(logger_, "setting shooter wheel to {} ticks/100ms",
               speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
  UpdateSmartDashboard();
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
  SPDLOG_DEBUG(logger_, "setting shooter wheel to {} ticks/100ms",
               speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
  UpdateSmartDashboard();
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
  SPDLOG_DEBUG(logger_, "setting shooter wheel to {} ticks/100ms",
               speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
  UpdateSmartDashboard();
}

/**
 * GetSpeedSetpoint returns the current speed setpoint.
 * @returns speed in encoder ticks per 100 ms
 */
int Shooter::GetSpeedSetpoint() const { return speed_setpoint_; }

/**
 * GetSpeed returns the current encoder speed.
 * @returns speed in encoder ticks per 100 ms
 */
int Shooter::GetSpeed() const {
  auto speed = RobotMap::shooter_wheel_talon->GetSpeed();
  return static_cast<int>(speed);
}

namespace {
const std::string kWheelSpeedKey("Wheel Speed");
const std::string kWheelElevationKey("Shooter Elevation");
}

/**
 * Send elevation and speed to SmartDashboard.
 */
void Shooter::UpdateSmartDashboard() {
#if !NDEBUG
  SmartDashboard::PutNumber(kWheelSpeedKey, speed_setpoint_);
  SmartDashboard::PutNumber(kWheelElevationKey, elevation_setpoint_);
#endif
}

/**
 * Check wheel speed against limits.
 */
int Shooter::LimitSpeed(int speed) {
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

/**
 * Check shooter elevation against limits.
 */
int Shooter::LimitElevation(int elevation) {
  if (elevation < kMinElevation) {
    logger_->warn("shooter elevation out of range: {}", elevation);
    elevation = kMinElevation;
  }
  if (elevation > kMaxElevation) {
    logger_->warn("shooter elevation out of range: {}", elevation);
    elevation = kMaxElevation;
  }
  return elevation;
}

void Shooter::LoadConfigSettings(
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

  auto d_opt = config->get_as<double>("elevation_zero_angle");
  if (d_opt) {
    elevation_zero_angle_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER elevation_zero_angle setting not available, using "
        "default");
  }
  logger_->info("shooter elevation zero angle: {}", elevation_zero_angle_);

  d_opt = config->get_as<double>("centerline_height");
  if (d_opt) {
    centerline_height_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER centerline_height setting not available, using "
        "default");
  }
  logger_->info("shooter target centerline height: {}", centerline_height_);

  d_opt = config->get_as<double>("camera_height");
  if (d_opt) {
    camera_height_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER camera_height setting not available, using "
        "default");
  }
  logger_->info("shooter camera height: {}", camera_height_);

  d_opt = config->get_as<double>("camera_angle");
  if (d_opt) {
    camera_angle_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER camera_angle setting not available, using "
        "default");
  }
  logger_->info("shooter camera angle: {}", camera_angle_);

  d_opt = config->get_as<double>("degrees_per_tick");
  if (d_opt) {
    degrees_per_tick_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER degrees_per_tick setting not available, using "
        "default");
  }
  logger_->info("shooter degrees per tick: {}", degrees_per_tick_);

  // configure talons
  auto elevation_settings =
      talon::Settings::Create(config, "shooter_elevation");
  elevation_settings->Initialize(RobotMap::shooter_elevation_talon);
  logger_->info("shooter elevation Talon initialized");

  auto speed_settings = talon::Settings::Create(config, "shooter_wheel");
  speed_settings->Initialize(RobotMap::shooter_wheel_talon);
  logger_->info("shooter wheel Talon initialized");
}
