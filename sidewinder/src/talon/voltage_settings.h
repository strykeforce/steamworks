#pragma once

#include "cpptoml/cpptoml.h"
#include "ctrlib/CANTalon.h"
#include "spdlog/spdlog.h"

#include "settings.h"

namespace sidewinder {
namespace talon {

/** Represents a Talon configured for open-loop voltage compensation control.
 */
class VoltageSettings : public Settings {
 public:
  VoltageSettings(const std::shared_ptr<cpptoml::table> config);
  virtual ~VoltageSettings() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
