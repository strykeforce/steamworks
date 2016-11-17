#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "settings.h"

namespace sidewinder {
namespace talon {

/** Represents a Talon configured for closed-loop control.
 * This class is not intended to be used directly, use a subclass.
 */
class PIDSettings : public Settings {
 private:
  double p_, i_, d_, f_;
  // TODO: verify uint types
  uint32_t i_zone_;
  uint32_t allowable_closed_loop_error_;
  double close_loop_ramp_rate_;
  double peak_output_voltage_forward_, peak_output_voltage_reverse_;
  double nominal_output_voltage_forward_, nominal_output_voltage_reverse_;

 public:
  PIDSettings(const std::shared_ptr<cpptoml::table> config);
  virtual ~PIDSettings() = default;
  virtual void SetMode(::CANTalon* talon) const override;
  virtual void LogConfig(
      const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
