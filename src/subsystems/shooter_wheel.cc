#include "shooter_wheel.h"

#include <cmath>

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

// TODO: put these in config
namespace {
const int kSpeedIncrement = 10;
const int kMinSpeed = 350;
const int kMaxSpeed = 700;
}

/**
 * ShooterWheel constructor.
 */
ShooterWheel::ShooterWheel(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("ShooterWheel"), logger_(spdlog::get("subsystem")) {
  LoadConfigSettings(config);
}

/**
 * SetSpeed sets the shooter wheel speed.
 */
void ShooterWheel::SetSpeed(int speed) {
  speed_setpoint_ = speed;
  // SPDLOG_DEBUG(logger_, "setting shooter wheel to {} ticks/100ms",
  //              speed_setpoint_);
  RobotMap::shooter_wheel_talon->Set(static_cast<double>(speed_setpoint_));
  UpdateSmartDashboard();
}

/**
 * SpeedIncrement raises the elevation by a small constant amount.
 */
void ShooterWheel::IncrementSpeed() {
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
void ShooterWheel::DecrementSpeed() {
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
int ShooterWheel::GetSpeedSetpoint() const { return speed_setpoint_; }

/**
 * GetSpeed returns the current encoder speed.
 * @returns speed in encoder ticks per 100 ms
 */
int ShooterWheel::GetSpeed() const {
  auto speed = RobotMap::shooter_wheel_talon->GetSpeed();
  return static_cast<int>(speed);
}

namespace {
const std::string kWheelSpeedKey("Wheel Speed");
}

/**
 * Send elevation and speed to SmartDashboard.
 */
void ShooterWheel::UpdateSmartDashboard() {
#if !NDEBUG
  SmartDashboard::PutNumber(kWheelSpeedKey, speed_setpoint_);
#endif
}

/**
 * Check wheel speed against limits.
 */
int ShooterWheel::LimitSpeed(int speed) {
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

void ShooterWheel::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("SHOOTER");
  if (!config) {
    throw std::invalid_argument("STEAMWORKS.SHOOTER table missing from config");
  }

  auto speed_settings = talon::Settings::Create(config, "shooter_wheel");
  speed_settings->Initialize(RobotMap::shooter_wheel_talon);
  logger_->info("shooter wheel Talon initialized");
}
