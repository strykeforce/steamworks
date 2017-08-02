#include "position_settings.h"

#include "cpptoml/cpptoml.h"
#include "ctrlib/CANTalon.h"
#include "spdlog/spdlog.h"

#include "pid_settings.h"

using namespace sidewinder::talon;

PositionSettings::PositionSettings(const std::shared_ptr<cpptoml::table> config)
    : PIDSettings(config) {}

void PositionSettings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetTalonControlMode(::CANTalon::kPositionMode);
  PIDSettings::SetMode(talon);
}

void PositionSettings::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  assert(logger);
  logger->debug("control mode = position");
  PIDSettings::LogConfig(logger);
}
