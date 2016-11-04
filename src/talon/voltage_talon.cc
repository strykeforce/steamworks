#include "voltage_talon.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "talon.h"

namespace sidewinder {
namespace talon {
VoltageTalon::VoltageTalon(const std::shared_ptr<cpptoml::table> config)
    : Talon(config) {}

VoltageTalon::~VoltageTalon() {}

void VoltageTalon::SetMode(::CANTalon* talon) const {
  talon->SetControlMode(::CANTalon::ControlMode::kVoltage);
  Talon::SetMode(talon);
}
} /* talon */
} /* sidewinder */
