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
  auto steamworks_config = config->get_table("STEAMWORKS");

  auto voltage = steamworks_config->get_as<double>("intake_voltage");
  if (voltage) {
    voltage_ = *voltage;
  } else {
    logger_->warn("STEAMWORKS intake_voltage setting missing, using default");
  }
  logger_->info("intake motor voltage: {}", voltage_);

  auto talon_settings = talon::Settings::Create(steamworks_config, "intake");
  logger_->debug("dumping intake talon configuration");
  talon_settings->LogConfig(logger_);
  talon_settings->Initialize(RobotMap::intake_talon);
}

void Intake::Start() { RobotMap::intake_talon->Set(voltage_); }

void Intake::Reverse() { RobotMap::intake_talon->Set(-voltage_); }

void Intake::Stop() { RobotMap::intake_talon->Set(0.0); }

int Intake::GetSpeed() {
  return static_cast<int>(RobotMap::intake_talon->GetSpeed());
}
