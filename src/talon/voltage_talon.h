#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "talon.h"

namespace sidewinder {
namespace talon {

class VoltageTalon : public Talon {
 private:
 public:
  // double p, i, d, f;
  // uint32_t i_zone, allowable_closed_loop_error;
  // double close_loop_ramp_rate, peak_output_voltage_forward,
  //     peak_output_voltage_reverse;

  VoltageTalon(const std::shared_ptr<cpptoml::table> config);
  virtual ~VoltageTalon();
  // virtual void Configure(::CANTalon* talon) const override;
  virtual void SetMode(::CANTalon* talon) const override;
  // virtual void DumpToLog(
  //     const std::shared_ptr<spdlog::logger> logger) const override;
};

} /* talon */
} /* sidewinder */
