#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace sidewinder {
namespace talon {

class Talon {
 private:
  ::CANTalon::FeedbackDevice feedback_device_;
  ::CANTalon::NeutralMode neutral_mode_;
  ::CANTalon::LimitMode limit_mode_;
  double voltage_ramp_rate_;
  bool encoder_reversed_;
  bool output_reversed_;

 public:
  Talon(const std::shared_ptr<cpptoml::table> config);
  virtual ~Talon();
  virtual void Configure(::CANTalon* talon) const;
  virtual void SetMode(::CANTalon* talon) const;
  virtual void LogConfig(const std::shared_ptr<spdlog::logger> logger) const;
};

} /* talon */
} /* sidewinder */
