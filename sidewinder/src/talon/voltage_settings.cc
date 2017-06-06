#include "voltage_settings.h"

#include "CANTalon.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "settings.h"

using namespace sidewinder::talon;

VoltageSettings::VoltageSettings(const std::shared_ptr<cpptoml::table> config)
    : Settings(config) {}

void VoltageSettings::SetMode(::CANTalon* talon) const {
  talon->SetTalonControlMode(::CANTalon::kVoltageMode);
  Settings::SetMode(talon);
}

void VoltageSettings::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  logger->debug("control mode = voltage");
  Settings::LogConfig(logger);
}
