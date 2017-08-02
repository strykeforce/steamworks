#pragma once

#include <cpptoml/cpptoml.h>
#include <ctrlib/CANTalon.h>
#include <spdlog/spdlog.h>

#include "pid_settings.h"

namespace sidewinder {
namespace talon {

/** Represents a Talon configured for closed-loop motion magic control.
 */
class MotionMagicSettings : public PIDSettings {
 public:
  MotionMagicSettings(const std::shared_ptr<cpptoml::table> config);
  virtual ~MotionMagicSettings() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;

 private:
  double cruise_velocity_;
  double acceleration_;
};

} /* talon */
} /* sidewinder */
