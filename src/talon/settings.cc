#include "settings.h"

#include <iostream>
#include "CANTalon.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "position_settings.h"
#include "velocity_settings.h"
#include "voltage_settings.h"

using namespace sidewinder::talon;
using namespace std;

/** Factory method to find Talon settinsand return appropriate subclass of
 * Talon. This gets its settings from the config file based on "name" and
 * sets mode based on "type", for example, this config file section:
 *
 *       [[SIDEWINDER.SWERVE.TALON]]
 *       name = "azimuth"
 *       type = "position"
 */
unique_ptr<Settings> Settings::Create(
    const std::shared_ptr<cpptoml::table> config, const string name) {
  assert(config);
  assert(name != "");

  // get array of Talon config tables
  auto talons = config->get_table_array("TALON");
  if (!talons) {
    throw invalid_argument("TALON table array missing");
  }

  // find Talon config with specified name
  auto t_itr = find_if(begin(*talons), end(*talons),
                       [=](const std::shared_ptr<cpptoml::table> c) {
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
    settings.reset(new VoltageSettings(talon));
  } else if (*type == "position") {
    settings.reset(new PositionSettings(talon));
  } else if (*type == "velocity") {
    settings.reset(new VelocitySettings(talon));
  } else {
    throw invalid_argument("unrecognized type: " + *type);
  }

  return settings;
}

/** Construct a Talon Settings object.
 */
Settings::Settings(const std::shared_ptr<cpptoml::table> config) {
  assert(config);

  // configure settings common to all operating modes.

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

/** Configure is intended to be one-time setup for Talons at initialization.
 */
void Settings::Configure(::CANTalon* talon) const {
  assert(talon);
  talon->SelectProfileSlot(0);
  talon->SetFeedbackDevice(feedback_device_);
  talon->ConfigNeutralMode(neutral_mode_);
  talon->ConfigLimitMode(limit_mode_);
  talon->SetSensorDirection(encoder_reversed_);
  talon->SetInverted(output_reversed_);
}

/** SetMode sets the current operating mode for the Talon, i.e. voltage,
 * velocity, etc. It is not unusual to switch between modes frequently during
 * robot operation.
 */
void Settings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetVoltageRampRate(voltage_ramp_rate_);
}

/** Convenience method that calls Configure and SetMode for talon.
 */
void Settings::Initialize(::CANTalon* talon) const {
  Configure(talon);
  SetMode(talon);
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
