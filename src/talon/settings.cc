#include "settings.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "position_settings.h"
#include "voltage_settings.h"

using namespace sidewinder::talon;
using namespace std;

/** Find Talon settins with given name and return appropriate subclass of Talon.
 */
unique_ptr<Settings> Settings::Create(TalonConfig config, const string name) {
  assert(config);
  assert(name != "");

  // get array of Talon config tables
  auto talons = config->get_table_array("TALON");
  if (!talons) {
    throw invalid_argument("TALON table array missing");
  }

  // find Talon config with specified name
  auto t_itr = find_if(begin(*talons), end(*talons), [=](TalonConfig c) {
    return name == c->get_as<string>("name").value_or("");
  });
  if (t_itr == end(*talons)) {
    throw invalid_argument("TALON not found for name: " + name);
  }

  // find type of found config: voltage, position, etc
  auto talon = *t_itr;
  auto type = talon->get_as<string>("type");
  if (!talon) {
    throw invalid_argument("TALON type key missing");
  }

  // create configured Talon settings of specified type
  unique_ptr<Settings> settings{nullptr};
  if (*type == "voltage") {
    settings.reset(new VoltageSettings(config));
  } else if (*type == "position") {
    settings.reset(new PositionSettings(config));
  } else if (*type == "velocity") {
    throw invalid_argument("velocity talon not implemented");
  } else {
    throw invalid_argument("unrecognized type: " + *type);
  }

  return settings;
}

Settings::Settings(TalonConfig config) {
  assert(config);
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

void Settings::Configure(::CANTalon* talon) const {
  assert(talon);
  talon->SelectProfileSlot(0);
  talon->SetFeedbackDevice(feedback_device_);
  talon->ConfigNeutralMode(neutral_mode_);
  talon->ConfigLimitMode(limit_mode_);
  talon->SetSensorDirection(encoder_reversed_);
  talon->SetInverted(output_reversed_);
}

void Settings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetVoltageRampRate(voltage_ramp_rate_);
}

void Settings::LogConfig(Logger logger) const {
  assert(logger);
  logger->debug("feedback_device = {}", feedback_device_);
  logger->debug("neutral_mode = {}", neutral_mode_);
  logger->debug("limit_mode = {}", limit_mode_);
  logger->debug("voltage_ramp_rate = {} v/sec", voltage_ramp_rate_);
  logger->debug("encoder_reversed = {}", encoder_reversed_);
  logger->debug("output_reversed = {}", output_reversed_);
}
