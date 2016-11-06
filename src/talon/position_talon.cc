#include "position_talon.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "pid_talon.h"

namespace sidewinder {
namespace talon {
PositionTalon::PositionTalon(const std::shared_ptr<cpptoml::table> config)
    : PIDTalon(config) {}

PositionTalon::~PositionTalon() {}

void PositionTalon::SetMode(::CANTalon* talon) const {
  talon->SetControlMode(::CANTalon::ControlMode::kPosition);
  PIDTalon::SetMode(talon);
}

void PositionTalon::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  logger->debug("control mode = position");
  PIDTalon::LogConfig(logger);
}
} /* talon */
} /* sidewinder */
