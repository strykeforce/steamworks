#pragma once

#include "CANTalon.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "pid_settings.h"

namespace sidewinder {
namespace talon {

/** Represents a Talon configured for closed-loop position control.
 */
class PositionSettings : public PIDSettings {
 public:
  PositionSettings(const std::shared_ptr<cpptoml::table> config);
  virtual ~PositionSettings() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
