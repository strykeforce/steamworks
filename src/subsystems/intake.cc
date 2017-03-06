#include "intake.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/talon/settings.h"

#include "robot_map.h"
#include "sidewinder/talon/settings.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

Intake::Intake(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Intake"),
      logger_(spdlog::get("subsystem")),
      voltage_(12.0) {
  LoadConfigSettings(config);
}

void Intake::Start() { RobotMap::intake_talon->Set(voltage_); }

void Intake::Reverse() { RobotMap::intake_talon->Set(-voltage_); }

void Intake::Stop() { RobotMap::intake_talon->StopMotor(); }

int Intake::GetSpeed() {
  return static_cast<int>(RobotMap::intake_talon->GetSpeed());
}

/**
 * Load configuration
 */
void Intake::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("INTAKE");
  if (!config) {
    throw std::invalid_argument("STEAMWORKS.INTAKE table missing from config");
  }

  auto voltage = config->get_as<double>("voltage");
  if (voltage) {
    voltage_ = *voltage;
  } else {
    logger_->warn("STEAMWORKS.INTAKE voltage setting missing, using default");
  }
  logger_->info("intake motor voltage: {}", voltage_);

  auto talon_settings = talon::Settings::Create(config, "intake");
  // SPDLOG_DEBUG(logger_, "dumping intake talon configuration");
  // talon_settings->LogConfig(logger_);
  talon_settings->Initialize(RobotMap::intake_talon);
  logger_->info("intake Talon initialized");
}
