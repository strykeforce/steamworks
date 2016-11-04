#include "talon.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace sidewinder {
namespace talon {

Talon::Talon(const std::shared_ptr<cpptoml::table> config) {
  feedback_device_ = static_cast<::CANTalon::FeedbackDevice>(
      *config->get_as<int>("feedback_device"));

  neutral_mode_ = static_cast<::CANTalon::NeutralMode>(
      *config->get_as<int>("neutral_mode"));

  limit_mode_ =
      static_cast<::CANTalon::LimitMode>(*config->get_as<int>("limit_mode"));

  voltage_ramp_rate_ = *config->get_as<double>("voltage_ramp_rate");
  encoder_reversed_ = *config->get_as<bool>("encoder_reversed");
  output_reversed_ = *config->get_as<bool>("output_reversed");
}

Talon::~Talon() {}

void Talon::Configure(::CANTalon* talon) const {
  talon->SelectProfileSlot(0);
  talon->SetFeedbackDevice(feedback_device_);
  talon->ConfigNeutralMode(neutral_mode_);
  talon->ConfigLimitMode(limit_mode_);
  talon->SetSensorDirection(encoder_reversed_);
  talon->SetInverted(output_reversed_);
}

void Talon::SetMode(::CANTalon* talon) const {
  talon->SetVoltageRampRate(voltage_ramp_rate_);
}

void Talon::DumpToLog(const std::shared_ptr<spdlog::logger> logger) const {
  logger->debug("feedback_device = {}", feedback_device_);
  logger->debug("neutral_mode = {}", neutral_mode_);
  logger->debug("limit_mode = {}", limit_mode_);
  logger->debug("voltage_ramp_rate = {}", voltage_ramp_rate_);
  logger->debug("encoder_reversed = {}", encoder_reversed_);
  logger->debug("output_reversed = {}", output_reversed_);
}
} /* talon */
} /* sidewinder */
