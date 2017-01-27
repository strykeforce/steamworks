#pragma once

#include "CANTalon.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace sidewinder {
namespace talon {

using Logger = const std::shared_ptr<spdlog::logger>;

/** Base class representing a Talon configuration.
 * This class is not intended to be used directly, use a subclass.
 */
class Settings {
 private:
  ::CANTalon::FeedbackDevice feedback_device_;
  ::CANTalon::NeutralMode neutral_mode_;
  ::CANTalon::LimitMode limit_mode_;
  double voltage_ramp_rate_;
  bool encoder_reversed_;
  bool output_reversed_;

 public:
  static std::unique_ptr<Settings> Create(
      const std::shared_ptr<cpptoml::table> config, const std::string name);
  Settings(const std::shared_ptr<cpptoml::table> config);
  virtual ~Settings() = default;
  virtual void Configure(::CANTalon* talon) const;
  virtual void SetMode(::CANTalon* talon) const;
  virtual void LogConfig(Logger logger) const;
};

} /* talon */
} /* sidewinder */