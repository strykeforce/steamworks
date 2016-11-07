#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "talon.h"

namespace sidewinder {
namespace talon {

class PIDTalon : public Talon {
 private:
  double p_, i_, d_, f_;
  // TODO: verify uint types
  uint32_t i_zone_;
  uint32_t allowable_closed_loop_error_;
  double close_loop_ramp_rate_;
  double peak_output_voltage_forward_, peak_output_voltage_reverse_;
  double nominal_output_voltage_forward_, nominal_output_voltage_reverse_;

 public:
  PIDTalon(const std::shared_ptr<cpptoml::table> config);
  virtual ~PIDTalon() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
