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
  // factory method, used to create obj based on config file
  static std::unique_ptr<Settings> Create(
      const std::shared_ptr<cpptoml::table> config, const std::string name);

  Settings(const std::shared_ptr<cpptoml::table> config);
  virtual ~Settings() = default;

  // one time configuration
  virtual void Configure(::CANTalon* talon) const;
  // set mode (voltage, etc) during operation
  virtual void SetMode(::CANTalon* talon) const;
  // convience method, calls Configure and SetMode
  void Initialize(::CANTalon* talon) const;
  // dump config to log
  virtual void LogConfig(Logger logger) const;
};

} /* talon */
} /* sidewinder */
