#include "pid_settings.h"

#include "cpptoml/cpptoml.h"
#include "ctrlib/CANTalon.h"
#include "spdlog/spdlog.h"

#include "settings.h"

using namespace sidewinder::talon;
using namespace std;

PIDSettings::PIDSettings(const std::shared_ptr<cpptoml::table> config)
    : Settings(config) {
  auto d_opt = config->get_as<double>("P");
  if (!d_opt) {
    throw invalid_argument("TALON P setting missing");
  }
  p_ = *d_opt;

  d_opt = config->get_as<double>("I");
  if (!d_opt) {
    throw invalid_argument("TALON I setting missing");
  }
  i_ = *d_opt;

  d_opt = config->get_as<double>("D");
  if (!d_opt) {
    throw invalid_argument("TALON D setting missing");
  }
  d_ = *d_opt;

  d_opt = config->get_as<double>("F");
  if (!d_opt) {
    throw invalid_argument("TALON F setting missing");
  }
  f_ = *d_opt;

  auto ui_opt = config->get_as<unsigned>("I_zone");
  if (!ui_opt) {
    throw invalid_argument("TALON I_zone setting missing");
  }
  i_zone_ = *ui_opt;

  ui_opt = config->get_as<uint32_t>("allowable_closed_loop_error");
  if (!ui_opt) {
    throw invalid_argument("TALON allowable_closed_loop_error setting missing");
  }
  allowable_closed_loop_error_ = *ui_opt;

  d_opt = config->get_as<double>("close_loop_ramp_rate");
  if (!d_opt) {
    throw invalid_argument("TALON close_loop_ramp_rate setting missing");
  }
  close_loop_ramp_rate_ = *d_opt;

  d_opt = config->get_as<double>("peak_output_voltage_forward");
  if (!d_opt) {
    throw invalid_argument("TALON peak_output_voltage_forward setting missing");
  }
  peak_output_voltage_forward_ = *d_opt;

  d_opt = config->get_as<double>("peak_output_voltage_reverse");
  if (!d_opt) {
    throw invalid_argument("TALON peak_output_voltage_reverse setting missing");
  }
  peak_output_voltage_reverse_ = *d_opt;

  d_opt = config->get_as<double>("nominal_output_voltage_forward");
  if (!d_opt) {
    throw invalid_argument(
        "TALON nominal_output_voltage_forward setting missing");
  }
  nominal_output_voltage_forward_ = *d_opt;

  d_opt = config->get_as<double>("nominal_output_voltage_reverse");
  if (!d_opt) {
    throw invalid_argument(
        "TALON nominal_output_voltage_reverse setting missing");
  }
  nominal_output_voltage_reverse_ = *d_opt;
}

void PIDSettings::SetMode(::CANTalon* talon) const {
  assert(talon);
  // override to set control mode
  Settings::SetMode(talon);
  talon->SetPID(p_, i_, d_, f_);
  talon->SetIzone(i_zone_);
  talon->SetAllowableClosedLoopErr(allowable_closed_loop_error_);
  talon->ConfigPeakOutputVoltage(peak_output_voltage_forward_,
                                 peak_output_voltage_reverse_);
  talon->ConfigNominalOutputVoltage(nominal_output_voltage_forward_,
                                    nominal_output_voltage_reverse_);
  talon->SetCloseLoopRampRate(close_loop_ramp_rate_);
}

void PIDSettings::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  assert(logger);
  logger->debug("P = {}", p_);
  logger->debug("I = {}", i_);
  logger->debug("D = {}", d_);
  logger->debug("F = {}", f_);
  logger->debug("I Zone = {}", i_zone_);
  logger->debug("allowable closed loop error = {}",
                allowable_closed_loop_error_);
  logger->debug("peak output voltage (F/R) = {} v, {} v",
                peak_output_voltage_forward_, peak_output_voltage_reverse_);
  logger->debug("nominal output voltage (F/R) = {} v, {} v",
                nominal_output_voltage_forward_,
                nominal_output_voltage_reverse_);
  logger->debug("closed loop ramp rate = {} v/sec", close_loop_ramp_rate_);
  Settings::LogConfig(logger);
}
