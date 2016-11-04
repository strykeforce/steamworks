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
  //  double p, i, d, f;
  //  uint32_t i_zone, allowable_closed_loop_error;
  //  double close_loop_ramp_rate, peak_output_voltage_forward,
  //      peak_output_voltage_reverse;

  Talon(const std::shared_ptr<cpptoml::table> config);
  virtual ~Talon();
  virtual void Configure(::CANTalon* talon) const;
  virtual void SetMode(::CANTalon* talon) const;
  virtual void DumpToLog(const std::shared_ptr<spdlog::logger> logger) const;
};

} /* talon */
} /* sidewinder */
