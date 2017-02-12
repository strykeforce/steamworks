#include "pid_settings.h"

#include "CANTalon.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "settings.h"

using namespace sidewinder::talon;

PIDSettings::PIDSettings(const std::shared_ptr<cpptoml::table> config)
    : Settings(config) {
  p_ = config->get_as<double>("P").value_or(0.0);
  i_ = config->get_as<double>("I").value_or(0.0);
  d_ = config->get_as<double>("D").value_or(0.0);
  f_ = config->get_as<double>("F").value_or(0.0);
  i_zone_ = config->get_as<unsigned>("I_zone").value_or(0);

  allowable_closed_loop_error_ =
      config->get_as<uint32_t>("allowable_closed_loop_error").value_or(0);

  close_loop_ramp_rate_ =
      config->get_as<double>("close_loop_ramp_rate").value_or(0.0);

  peak_output_voltage_forward_ =
      config->get_as<double>("peak_output_voltage_forward").value_or(4.0);
  peak_output_voltage_reverse_ =
      config->get_as<double>("peak_output_voltage_reverse").value_or(4.0);

  nominal_output_voltage_forward_ =
      config->get_as<double>("nominal_output_voltage_forward").value_or(0.0);
  nominal_output_voltage_reverse_ =
      config->get_as<double>("nominal_output_voltage_reverse").value_or(0.0);
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