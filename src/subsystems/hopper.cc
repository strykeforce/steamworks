#include "hopper.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "robot_map.h"
#include "sidewinder/talon/settings.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

Hopper::Hopper(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Hopper"), logger_(spdlog::get("subsystem")) {
  // FIXME: get voltage
  auto steamworks_config = config->get_table("STEAMWORKS");

  auto talon_settings = talon::Settings::Create(steamworks_config, "hopper");
  logger_->debug("dumping hopper talon configuration");
  talon_settings->LogConfig(logger_);
  talon_settings->Configure(RobotMap::hopper_talon);
  talon_settings->SetMode(RobotMap::hopper_talon);
}

void Hopper::Start() { RobotMap::hopper_talon->Set(7.0); }

void Hopper::Stop() { RobotMap::hopper_talon->Set(0.0); }
