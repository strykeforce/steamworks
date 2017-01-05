#include "velocity_settings.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "pid_settings.h"

using namespace sidewinder::talon;

VelocitySettings::VelocitySettings(const std::shared_ptr<cpptoml::table> config)
    : PIDSettings(config) {}

void VelocitySettings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetControlMode(::CANTalon::ControlMode::kSpeed);
  PIDSettings::SetMode(talon);
}

void VelocitySettings::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  assert(logger);
  logger->debug("control mode = velocity");
  PIDSettings::LogConfig(logger);
}