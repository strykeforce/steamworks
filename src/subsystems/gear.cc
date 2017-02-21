#include "gear.h"

#include "robot_map.h"
#include "sidewinder/talon/settings.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

GearLoader::GearLoader(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("GearLoader"), logger_(spdlog::get("subsystem")) {
  auto steamworks_config = config->get_table("STEAMWORKS");

  auto voltage = steamworks_config->get_as<double>("gear_load_voltage");
  if (voltage) {
    load_voltage_ = *voltage;
  } else {
    logger_->warn(
        "STEAMWORKS gear_load_voltage setting missing, using default");
  }
  logger_->info("gear load voltage: {}", load_voltage_);

  voltage = steamworks_config->get_as<double>("gear_deploy_voltage");
  if (voltage) {
    deploy_voltage_ = *voltage;
  } else {
    logger_->warn(
        "STEAMWORKS gear_deploy_voltage setting missing, using default");
  }
  logger_->info("gear load voltage: {}", deploy_voltage_);

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
 * IsIntakeEnabled
 */
bool GearLoader::IsIntakeEnabled() {
  return RobotMap::gear_intake_talon->IsEnabled();
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
 * SetServo
 */
void GearLoader::SetServo(double value) {
  logger_->info("set servo to {}", value);
  servo_.Set(value);
}

/**
 * GetServo
 */
double GearLoader::GetServo() {
  double pos = servo_.Get();
  logger_->debug("servo position is {}", pos);
  return pos;
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
    logger_->info("starting pivot zero mode");
    RobotMap::gear_pivot_talon->StopMotor();
    pivot_zero_settings_->Initialize(RobotMap::gear_pivot_talon);
    RobotMap::gear_pivot_talon->Set(kGearZeroVoltage);
    return;
  }
  logger_->info("finished pivot zero mode");
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
 * SetPivotEncoderZero
 */
void GearLoader::SetPivotEncoderZero() {
  logger_->debug("setting pivot encoder position to {}", kGearZeroPosition);
  RobotMap::gear_pivot_talon->SetPosition(kGearZeroPosition);
}

/**
 * PivotUp
 */
void GearLoader::PivotUp() {
  logger_->info("pivoting gear to {}", kGearPivotUpPosition);
  RobotMap::gear_pivot_talon->Set(kGearPivotUpPosition);
}

/**
 * PivotDown
 */
void GearLoader::PivotDown() {
  logger_->info("pivoting gear to {}", kGearPivotDownPosition);
  RobotMap::gear_pivot_talon->Set(kGearPivotDownPosition);
}
