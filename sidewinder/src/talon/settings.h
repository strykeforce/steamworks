#pragma once

#include <cpptoml/cpptoml.h>
#include <ctrlib/CANTalon.h>
#include <spdlog/spdlog.h>

namespace sidewinder {
namespace talon {

using Logger = const std::shared_ptr<spdlog::logger>;

/** Base class representing a Talon configuration.
 * This class is not intended to be used directly, use a subclass.
 */
class Settings {
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
  // get the max setpoint
  double GetSetpointMax();
  // dump config to log
  virtual void LogConfig(Logger logger) const;

 private:
  ::CANTalon::FeedbackDevice feedback_device_;
  ::CANTalon::NeutralMode neutral_mode_;
  ::CANTalon::LimitMode limit_mode_;
  double voltage_ramp_rate_;
  bool encoder_reversed_;
  bool output_reversed_;
  uint32_t current_limit_;
  double setpoint_max_ = -1.0;
};

} /* talon */
} /* sidewinder */
