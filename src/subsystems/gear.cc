#include "gear.h"

#include "robot_map.h"
#include "sidewinder/talon/settings.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

GearLoader::GearLoader(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("GearLoader"), logger_(spdlog::get("subsystem")) {
  LoadConfigSettings(config);
}

/**
 * SetLimitSwitchNormallyOpen
 */
void GearLoader::SetLimitSwitchNormallyOpen(bool open) {
  logger_->debug("setting limit switch normally open to {}", open);
  RobotMap::gear_intake_talon->ConfigFwdLimitSwitchNormallyOpen(open);
}

/**
 * Load
 */
void GearLoader::Load() {
  logger_->debug("starting gear load");
  RobotMap::gear_intake_talon->Set(load_voltage_);
}

/**
 * Deploy
 */
void GearLoader::Deploy() {
  logger_->info("starting gear deploy");
  RobotMap::gear_intake_talon->Set(deploy_voltage_);
}

/**
 * StopLoader
 */
void GearLoader::StopLoader() {
  logger_->info("stopping gear loader");
  RobotMap::gear_intake_talon->Set(0.0);
}

/**
 * IsLimitSwitchClosed
 */
bool GearLoader::IsLimitSwitchClosed() {
  static bool was_closed = false;

  bool is_closed = RobotMap::gear_intake_talon->IsFwdLimitSwitchClosed() == 1;
  if (is_closed != was_closed) {
    logger_->debug("limit switch closed {}", is_closed);
    was_closed = is_closed;
  }

  return is_closed;
}

/**
 * Set gear clamp to open position for loading.
 */
void GearLoader::ClampStage() {
  logger_->debug("gear clamp stage, left = {}, right ={}", left_clamp_stage_,
                 right_clamp_stage_);
  left_servo_.Set(left_clamp_stage_);
  right_servo_.Set(right_clamp_stage_);
}

/**
 * Set gear clamp to clamped shut position for pivoting.
 */
void GearLoader::ClampShut() {
  logger_->debug("gear clamp shut, left = {}, right = {}", left_clamp_shut_,
                 right_clamp_shut_);
  left_servo_.Set(left_clamp_shut_);
  right_servo_.Set(right_clamp_shut_);
}

/**
 * Set gear clamp to fully open position for releasing gear on lifter.
 */
void GearLoader::ClampRelease() {
  logger_->debug("gear clamp release, left = {}, right = {}",
                 left_clamp_release_, right_clamp_release_);
  left_servo_.Set(left_clamp_release_);
  right_servo_.Set(right_clamp_release_);
}

/**
 * Set gear clamp to safe stowed position.
 */
void GearLoader::ClampStow() {
  logger_->debug("gear clamp stow, left = {}, right = {}", left_clamp_stow_,
                 right_clamp_stow_);
  left_servo_.Set(left_clamp_stow_);
  right_servo_.Set(right_clamp_stow_);
}

namespace {
const double kGearZeroPosition = -50.0;
const double kGearZeroVoltage = -1.0;
const int kGearPivotUpPosition = 1150;
const int kGearPivotDownPosition = 0;
}

/**
 * SetPivotZeroModeEnabled
 */
void GearLoader::SetPivotZeroModeEnabled(bool enabled) {
  if (enabled) {
    logger_->info("enabling pivot zero mode");
    RobotMap::gear_pivot_talon->StopMotor();
    pivot_zero_settings_->Initialize(RobotMap::gear_pivot_talon);
    RobotMap::gear_pivot_talon->Set(kGearZeroVoltage);
    return;
  }
  logger_->debug("disabling pivot zero mode");
  RobotMap::gear_pivot_talon->StopMotor();
  pivot_settings_->Initialize(RobotMap::gear_pivot_talon);
}

/**
 * GetPivotPosition
 */
int GearLoader::GetPivotPosition() {
  auto pos = RobotMap::gear_pivot_talon->GetPosition();
  logger_->debug("pivot encoder position is {}", pos);
  return static_cast<int>(pos);
}

/**
 * Calibrate the pivot encoder after kissing off in full down position.
 */
void GearLoader::SetPivotEncoderZero() {
  logger_->debug("setting pivot encoder position to {}", kGearZeroPosition);
  RobotMap::gear_pivot_talon->SetPosition(kGearZeroPosition);
}

/**
 * Position the deployed gear in the upright position for loading onto lift.
 */
void GearLoader::PivotUp() {
  // sanity check for control mode, being in voltage mode would be BAD
  auto mode = RobotMap::gear_pivot_talon->GetTalonControlMode();
  if (mode != ::CANTalon::kPositionMode) {
    logger_->critical("gear pivot talon not in position control mode");
    return;
  }
  logger_->info("pivoting gear to {}", kGearPivotUpPosition);
  RobotMap::gear_pivot_talon->Set(kGearPivotUpPosition);
}

/**
 * Put pivot in fully down position.
 */
void GearLoader::PivotDown() {
  logger_->info("pivoting gear to {}", kGearPivotDownPosition);
  RobotMap::gear_pivot_talon->Set(kGearPivotDownPosition);
}

/**
 * Return true when pivot is in fully upright position as specified in config.
 */
bool GearLoader::IsPivotUp() { return GetPivotPosition() > pivot_up_position_; }

/**
 * Return true when pivot is in fully down position as specified in config.
 */
bool GearLoader::IsPivotDown() {
  return GetPivotPosition() < pivot_down_position_;
}

/**
 * Load configuration settings.
 */
void GearLoader::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config) {
  auto steamworks_config = config->get_table("STEAMWORKS");

  // gear_load_voltage
  auto d_opt = steamworks_config->get_as<double>("gear_load_voltage");
  if (d_opt) {
    load_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS gear_load_voltage setting missing, using default");
  }
  logger_->info("gear intake motor load voltage: {}", load_voltage_);

  // gear_deploy_voltage
  d_opt = steamworks_config->get_as<double>("gear_deploy_voltage");
  if (d_opt) {
    deploy_voltage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS gear_deploy_voltage setting missing, using default");
  }
  logger_->info("gear intake motor deploy voltage: {}", deploy_voltage_);

  // gear_clamp_stage
  d_opt = steamworks_config->get_as<double>("left_gear_clamp_stage");
  if (d_opt) {
    left_clamp_stage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS left_gear_clamp_stage setting missing, using default");
  }
  logger_->info("left gear clamp stage position: {}", left_clamp_stage_);

  d_opt = steamworks_config->get_as<double>("right_gear_clamp_stage");
  if (d_opt) {
    right_clamp_stage_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS right_gear_clamp_stage setting missing, using default");
  }
  logger_->info("right gear clamp stage position: {}", right_clamp_stage_);

  // gear_clamp_shut
  d_opt = steamworks_config->get_as<double>("left_gear_clamp_shut");
  if (d_opt) {
    left_clamp_shut_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS left_gear_clamp_shut setting missing, using default");
  }
  logger_->info("left gear clamp shut position: {}", left_clamp_shut_);

  d_opt = steamworks_config->get_as<double>("right_gear_clamp_shut");
  if (d_opt) {
    right_clamp_shut_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS right_gear_clamp_shut setting missing, using default");
  }
  logger_->info("right gear clamp shut position: {}", right_clamp_shut_);

  // gear_clamp_release
  d_opt = steamworks_config->get_as<double>("left_gear_clamp_release");
  if (d_opt) {
    left_clamp_release_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS left_gear_clamp_release setting missing, using default");
  }
  logger_->info("left gear clamp release position: {}", left_clamp_release_);

  d_opt = steamworks_config->get_as<double>("right_gear_clamp_release");
  if (d_opt) {
    right_clamp_release_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS right_gear_clamp_release setting missing, using default");
  }
  logger_->info("right gear clamp release position: {}", right_clamp_release_);

  // gear_clamp_stow
  d_opt = steamworks_config->get_as<double>("left_gear_clamp_stow");
  if (d_opt) {
    left_clamp_stow_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS left_gear_clamp_stow setting missing, using default");
  }
  logger_->info("left gear clamp stow position: {}", left_clamp_stow_);

  d_opt = steamworks_config->get_as<double>("right_gear_clamp_stow");
  if (d_opt) {
    right_clamp_stow_ = *d_opt;
  } else {
    logger_->warn(
        "STEAMWORKS right_gear_clamp_stow setting missing, using default");
  }
  logger_->info("right gear clamp stow position: {}", right_clamp_stow_);

  // gear_pivot_up
  auto i_opt = steamworks_config->get_as<int>("gear_pivot_up");
  if (i_opt) {
    pivot_up_position_ = *i_opt;
  } else {
    logger_->warn("STEAMWORKS gear_pivot_up setting missing, using default");
  }
  logger_->info("gear pivot up position: {}", pivot_up_position_);

  // gear_pivot_up
  i_opt = steamworks_config->get_as<int>("gear_pivot_down");
  if (i_opt) {
    pivot_down_position_ = *i_opt;
  } else {
    logger_->warn("STEAMWORKS gear_pivot_down setting missing, using default");
  }
  logger_->info("gear pivot down position: {}", pivot_down_position_);

  // Talons
  auto loader_talon_settings =
      talon::Settings::Create(steamworks_config, "gear_loader");
  logger_->debug("dumping gear loader talon configuration");
  loader_talon_settings->LogConfig(logger_);
  loader_talon_settings->Initialize(RobotMap::gear_intake_talon);

  pivot_settings_ = talon::Settings::Create(steamworks_config, "gear_pivot");
  logger_->debug("dumping gear pivot talon configuration");
  pivot_settings_->LogConfig(logger_);

  pivot_zero_settings_ =
      talon::Settings::Create(steamworks_config, "gear_pivot_zero");
  logger_->debug("dumping gear pivot zero talon configuration");
  pivot_zero_settings_->LogConfig(logger_);
}
