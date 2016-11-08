#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "pid_talon.h"

namespace sidewinder {
namespace talon {

/** Represents a Talon configured for closed-loop position control.
 */
class PositionTalon : public PIDTalon {
 public:
  PositionTalon(const std::shared_ptr<cpptoml::table> config);
  virtual ~PositionTalon() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
