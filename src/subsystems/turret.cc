#include "turret.h"

#include <cassert>

#include "WPILib.h"
#include "spdlog/spdlog.h"
#include "talon/settings.h"

#include "commands/position_turret.h"
#include "robot.h"
#include "robot_map.h"

using namespace avenger;
using namespace sidewinder;

Turret::Turret(const std::shared_ptr<cpptoml::table> config)
    : ::Subsystem("Turret"), logger_(spdlog::stdout_color_st(GetName())) {
  assert(config);
  logger_->set_level(spdlog::level::trace);
  logger_->trace("starting constructor");

  drive_voltage_ = static_cast<float>(
      config->get_as<double>("turret_drive_voltage").value_or(2.0));

  auto talon_settings = talon::Settings::Create(config, "turret");
  logger_->debug("dumping turret talon configuration");
  talon_settings->LogConfig(logger_);
  talon_settings->Configure(RobotMap::turret_talon);
  talon_settings->SetMode(RobotMap::turret_talon);
}

void Turret::InitDefaultCommand() { SetDefaultCommand(new PositionTurret()); }

void Turret::Move(float elevation) {
  // logger_->debug("moving to {}", elevation);
  RobotMap::turret_talon->Set(elevation * drive_voltage_);
}
