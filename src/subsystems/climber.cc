#include "climber.h"

#include "robot_map.h"
#include "sidewinder/talon/settings.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

Climber::Climber(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Climber"), logger_(spdlog::get("subsystem")) {
  LoadConfigSettings(config);
}

namespace {
const double kCaptureRampRate = 8.0;
const int kClimbDistanceGoodEnough = 100;
const int kZeroCountReq = 3;
}

/**
 * Run in capture mode.
 */
void Climber::SetCaptureModeEnabled(bool enabled) {
  if (enabled) {
    logger_->info("starting climber capture mode");
    RobotMap::climber_master_talon->SetVoltageRampRate(kCaptureRampRate);
    RobotMap::climber_slave_talon->SetVoltageRampRate(kCaptureRampRate);
    return;
  }
  logger_->info("finished climber capture mode");
  RobotMap::climber_master_talon->SetVoltageRampRate(0.0);
  RobotMap::climber_slave_talon->SetVoltageRampRate(0.0);
}

/**
 * Run in ratchet mode.
 */
void Climber::SetRatchetModeEnabled(bool enabled) {
  is_ratchet_ = enabled;
  logger_->info("climber ratchet mode enabled = {}", is_ratchet_);
}

/**
 * Run in slow finish mode.
 */
void Climber::SetFinishModeEnabled(bool enabled) {
  if (enabled) {
    logger_->info(
        "starting climber finish climb mode, position {}, current limit {}",
        GetPosition(), finish_current_);
    RobotMap::climber_master_talon->SetCurrentLimit(finish_current_);
    RobotMap::climber_slave_talon->SetCurrentLimit(finish_current_);
  }
  logger_->info("finished climber finish climb mode, position {}",
                GetPosition());
}

// TODO: combine these climb methods, use voltage_
void Climber::StartCapture() {
  RobotMap::climber_master_talon->Set(capture_voltage_);
  RobotMap::climber_slave_talon->Set(capture_voltage_);
  is_running_ = true;
}

void Climber::StartClimb() {
  RobotMap::climber_master_talon->Set(climb_voltage_);
  RobotMap::climber_slave_talon->Set(climb_voltage_);
  RobotMap::climber_master_talon->SetCurrentLimit(climb_current_);
  RobotMap::climber_slave_talon->SetCurrentLimit(climb_current_);
  is_running_ = true;
}

void Climber::StartFinish() {
  RobotMap::climber_master_talon->Set(finish_voltage_);
  RobotMap::climber_slave_talon->Set(finish_voltage_);
  is_running_ = true;
}

void Climber::Stop() {
  is_running_ = false;
  if (is_ratchet_) {
    RobotMap::climber_master_talon->Set(ratchet_voltage_);
    RobotMap::climber_slave_talon->Set(ratchet_voltage_);
    return;
  }
  RobotMap::climber_master_talon->StopMotor();
  RobotMap::climber_slave_talon->StopMotor();
}

bool Climber::IsRunning() { return is_running_; }

/**
 * Returns true when motor current increases due to rope tension.
 */
bool Climber::IsCaptured() {
  if (is_captured_) return true;

  double master = RobotMap::climber_master_talon->GetOutputCurrent();
  double slave = RobotMap::climber_slave_talon->GetOutputCurrent();
  SPDLOG_DEBUG(logger_, "master current = {}, slave current = {}", master,
               slave);
  if ((master + slave) / 2.0 > capture_current_) {
    ZeroPosition();
    is_captured_ = true;
    return true;
  }
  return false;
}

/**
 * Returns true when climber has moved required distance.
 */
bool Climber::ShouldClimbFinish() {
  // wait a few iterations for Robot::drive->ZeroPosition() to "take"
  if (zero_count_ < kZeroCountReq) {
    zero_count_++;
    return false;
  }

  int pos = GetPosition();
  SPDLOG_DEBUG(logger_, "Climber position absolute error {}", pos);
  return pos > climb_distance_;
}

/**
 * Returns true when climber has moved required distance.
 */
bool Climber::IsClimbFinished() {
  // TODO: will look at motor current to detect
  return false;
}

/**
 * Resets climber motor encoder to zero in preparation for measuring distance
 * during climb.
 */
void Climber::ZeroPosition() {
  if (is_zeroed_) {
    return;
  }
  RobotMap::climber_slave_talon->SetPosition(0);
  is_zeroed_ = true;
}

/**
 * Returns the current encoder position.
 */
int Climber::GetPosition() {
  return static_cast<int>(RobotMap::climber_slave_talon->GetPosition());
}

/**
 * Load configuration
 */
void Climber::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("CLIMBER");
  if (!config) {
    throw std::invalid_argument("STEAMWORKS.CLIMBER table missing from config");
  }

  auto d_opt = config->get_as<double>("capture_voltage");
  if (d_opt) {
    capture_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER capture_voltage setting missing, using default");
  }
  logger_->info("climber capture motor voltage: {}", capture_voltage_);

  d_opt = config->get_as<double>("climb_voltage");
  if (d_opt) {
    climb_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER climb_voltage setting missing, using default");
  }
  logger_->info("climber climb motor voltage: {}", climb_voltage_);

  d_opt = config->get_as<double>("climb_current");
  if (d_opt) {
    climb_current_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER climb_current setting missing, using default");
  }
  logger_->info("climber climb motor current: {}", climb_current_);

  d_opt = config->get_as<double>("finish_voltage");
  if (d_opt) {
    finish_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER finish_voltage setting missing, using default");
  }
  logger_->info("climber finish motor voltage: {}", finish_voltage_);

  d_opt = config->get_as<double>("ratchet_voltage");
  if (d_opt) {
    ratchet_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER ratchet_voltage setting missing, using default");
  }
  logger_->info("climber ratchet motor voltage: {}", ratchet_voltage_);

  d_opt = config->get_as<double>("capture_current");
  if (d_opt) {
    capture_current_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER capture_current setting missing, using default");
  }
  logger_->info("climber capture current setpoint: {}", capture_current_);

  d_opt = config->get_as<double>("finish_current");
  if (d_opt) {
    finish_current_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER finish_current setting missing, using default");
  }
  logger_->info("climber finish current setpoint: {}", finish_current_);

  auto i_opt = config->get_as<int>("climb_distance");
  if (i_opt) {
    climb_distance_ = *i_opt;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER climb_distance setting missing, using default");
  }
  logger_->info("climber climb distance: {}", climb_distance_);

  // configure master
  auto master_settings = talon::Settings::Create(config, "master");
  // SPDLOG_DEBUG(logger_, "dumping climber master talon configuration");
  // master_settings->LogConfig(logger_);
  master_settings->Initialize(RobotMap::climber_master_talon);
  logger_->info("climber master Talon initialized");

  // configure slave
  auto slave_settings = talon::Settings::Create(config, "slave");
  // SPDLOG_DEBUG(logger_, "dumping climber slave talon configuration");
  // slave_settings->LogConfig(logger_);
  slave_settings->Initialize(RobotMap::climber_slave_talon);
  logger_->info("climber slave Talon initialized");

  // override motor direction for practice robot
  if (RobotMap::IsPracticeRobot()) {
    RobotMap::climber_master_talon->SetInverted(false);
    RobotMap::climber_slave_talon->SetInverted(false);
    logger_->info("climber Talons reconfigured for practice robot");
  }
}
