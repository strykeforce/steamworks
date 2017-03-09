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
}

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

void Climber::StartCapture() {
  RobotMap::climber_master_talon->Set(capture_voltage_);
  RobotMap::climber_slave_talon->Set(capture_voltage_);
  is_running_ = true;
}

void Climber::StartClimb() {
  RobotMap::climber_master_talon->Set(climb_voltage_);
  RobotMap::climber_slave_talon->Set(climb_voltage_);
  is_running_ = true;
}
void Climber::Stop() {
  RobotMap::climber_master_talon->StopMotor();
  RobotMap::climber_slave_talon->StopMotor();
  is_running_ = false;
}

bool Climber::IsRunning() { return is_running_; }

bool Climber::IsCaptured() {
  double master = RobotMap::climber_master_talon->GetOutputCurrent();
  double slave = RobotMap::climber_slave_talon->GetOutputCurrent();
  SPDLOG_DEBUG(logger_, "master current = {}, slave current = {}", master,
               slave);
  return (master + slave) / 2.0 > capture_current_;
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

  auto val = config->get_as<double>("capture_voltage");
  if (val) {
    capture_voltage_ = *val;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER capture_voltage setting missing, using default");
  }
  logger_->info("climber capture motor voltage: {}", capture_voltage_);

  val = config->get_as<double>("climb_voltage");
  if (val) {
    climb_voltage_ = *val;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER climb_voltage setting missing, using default");
  }
  logger_->info("climber climb motor voltage: {}", climb_voltage_);

  val = config->get_as<double>("capture_current");
  if (val) {
    capture_current_ = *val;
  } else {
    logger_->warn(
        "STEAMWORKS.CLIMBER capture_current setting missing, using default");
  }
  logger_->info("climber capture current setpoint: {}", capture_current_);

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
