#include "settings.h"

#include "motion_magic_settings.h"
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
  } else if (*type == "motion_magic") {
    settings.reset(new MotionMagicSettings(talon));
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
  auto i_opt = config->get_as<int>("feedback_device");
  if (!i_opt) {
    throw invalid_argument("TALON feedback_device setting missing");
  }
  feedback_device_ = static_cast<::CANTalon::FeedbackDevice>(*i_opt);

  i_opt = config->get_as<int>("neutral_mode");
  if (!i_opt) {
    throw invalid_argument("TALON neutral_mode setting missing");
  }
  neutral_mode_ = static_cast<::CANTalon::NeutralMode>(*i_opt);

  i_opt = config->get_as<int>("limit_mode");
  if (!i_opt) {
    throw invalid_argument("TALON limit_mode setting missing");
  }
  limit_mode_ = static_cast<::CANTalon::LimitMode>(*i_opt);

  auto d_opt = config->get_as<double>("voltage_ramp_rate");
  if (!d_opt) {
    throw invalid_argument("TALON voltage_ramp_rate setting missing");
  }
  voltage_ramp_rate_ = *d_opt;

  auto b_opt = config->get_as<bool>("encoder_reversed");
  if (!b_opt) {
    throw invalid_argument("TALON encoder_reversed setting missing");
  }
  encoder_reversed_ = *b_opt;

  b_opt = config->get_as<bool>("output_reversed");
  if (!b_opt) {
    throw invalid_argument("TALON output_reversed setting missing");
  }
  output_reversed_ = *b_opt;

  auto ui_opt = config->get_as<uint32_t>("current_limit");
  if (!ui_opt) {
    throw invalid_argument("TALON current_limit setting missing");
  }
  current_limit_ = *ui_opt;

  // optionally set setpoint scaling
  d_opt = config->get_as<double>("setpoint_max");
  if (d_opt) {
    setpoint_max_ = *d_opt;
  }
}

/**
 * Configure is intended to be one-time setup for Talons at initialization.
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

/**
 * SetMode sets the current operating mode for the Talon, i.e. voltage,
 * velocity, etc. It is not unusual to switch between modes frequently during
 * robot operation.
 */
void Settings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetCurrentLimit(current_limit_);
  talon->EnableCurrentLimit(current_limit_ > 0);
  talon->SetVoltageRampRate(voltage_ramp_rate_);
}

/**
 * Convenience method that calls Configure and SetMode for talon.
 */
void Settings::Initialize(::CANTalon* talon) const {
  Configure(talon);
  SetMode(talon);
}

/**
 * Returns the optional max setpoint for this Talon configuration. If this is
 * accessed, but not set by the config file, it will throw an exception.
 */
double Settings::GetSetpointMax() {
  if (setpoint_max_ == -1) {
    throw logic_error("setpoint_max not used in these Talon settings");
  }
  return setpoint_max_;
}

/**
 * Log the settings to the supplied logger.
 */
void Settings::LogConfig(Logger logger) const {
  assert(logger);
  logger->debug("setpoint_max = {} (-1 = not used)", setpoint_max_);
  logger->debug("feedback_device = {}", feedback_device_);
  logger->debug("neutral_mode = {}", neutral_mode_);
  logger->debug("limit_mode = {}", limit_mode_);
  logger->debug("voltage_ramp_rate = {} v/sec", voltage_ramp_rate_);
  logger->debug("encoder_reversed = {}", encoder_reversed_);
  logger->debug("output_reversed = {}", output_reversed_);
  logger->debug("current_limit = {}", current_limit_);
}
