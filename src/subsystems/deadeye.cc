#include "deadeye.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace deadeye;
using namespace std;

// TODO: look at Notify to run this loop; should reduce CPU load
Deadeye::Deadeye(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Deadeye"), logger_(spdlog::get("deadeye")) {
  LoadConfigSettings(config);

  // turn off camera LEDs at robot start-up
  RobotMap::gear_camera_led->Set(true);
  RobotMap::shooter_camera_led->Set(true);

  // start notification loop
  SPDLOG_TRACE(logger_, "starting Notifier thread");
  notifier_ = std::make_unique<Notifier>(&Deadeye::Communicate, this);
  notifier_->StartPeriodic(period_ / 1000.0);
}

/**
 * Stop the IO notification loop.
 */
Deadeye::~Deadeye() {
  notifier_->Stop();
  serial_->close();
}

/**
 * Set the current Deadeye mode.
 */
void Deadeye::SetMode(Mode mode) {
  std::lock_guard<std::mutex> lock(mutex_);
  new_mode_ = mode;
}

/**
 * Get the current Deadeye mode.
 */
Mode Deadeye::GetMode() {
  std::lock_guard<std::mutex> lock(mutex_);
  return mode_;
}

/**
 * Main event loop, called by Notifier.
 */
void Deadeye::Communicate() {
  std::lock_guard<std::mutex> lock(mutex_);
  switch (state_) {
    case State::init_mode:
      state_ = DoInitMode();
      break;
    case State::boiler:
      state_ = DoBoiler();
      break;
    case State::gear:
      state_ = DoGear();
      break;
    case State::init:
      state_ = DoInit();
      break;
    case State::idle:
      state_ = DoIdle();
      break;
    case State::error:
      state_ = DoError();
      break;
  }
}

/**
 * Check for mode switch.
 */
inline Deadeye::State Deadeye::CheckMode() {
  if (mode_ == new_mode_) {
    return state_;  // stay in current state
  }
  mode_ = new_mode_;
  switch (mode_) {
    case Mode::boiler:
    case Mode::gear:
      return State::init_mode;
    case Mode::idle:
      return State::idle;
  }
  return State::error;  // shouldn't get here
}

/**
 * Handle initialization state.
 */
Deadeye::State Deadeye::DoInit() {
  SPDLOG_TRACE(logger_, "entering State::init");
  serial::Timeout timeout{0, 100, 0, 0, 0};
  try {
    serial_ = std::make_unique<serial::Serial>(port_, speed_, timeout);
    serial_->flushInput();
  } catch (const std::exception& e) {
    logger_->critical(e.what());
    return State::error;
  }
  return State::idle;
}

/**
 * Handle idle state.
 */
Deadeye::State Deadeye::DoIdle() {
  SPDLOG_TRACE(logger_, "entering State::idle");
  error_reported_ = false;
  return CheckMode();
}

/**
 * Handle boiler initialization.
 */
Deadeye::State Deadeye::DoInitMode() {
  SPDLOG_TRACE(logger_, "entering State::init_mode");
  ModeSentence mode(mode_);
  SendSentence(mode);
  // TODO: go into verify mode to read back matching sentence from Deadeye
  switch (mode_) {
    case Mode::boiler:
      return State::boiler;
    case Mode::gear:
      return State::gear;
    case Mode::idle:
      return State::idle;
  }
  return State::error;  // shouldn't get here
}

/**
 * Handle boiler.
 */
Deadeye::State Deadeye::DoBoiler() {
  SPDLOG_TRACE(logger_, "entering State::boiler");
  if (ReadSentence() == Deadeye::State::error) {
    return Deadeye::State::error;
  }
  if (sentence_.Valid()) {
    switch (sentence_.Code()) {
      case Sentence::kBoiler:
        azimuth_error_ = stoi(sentence_.parameters[1]);
        centerline_error_ = stoi(sentence_.parameters[2]);
        break;
      case Sentence::kNoTarget:
        SPDLOG_TRACE(logger_, "no target");
        break;
      case Sentence::kGear:
        throw std::logic_error("DoBoiler got GearSentence");
        break;
    }
  }
  return CheckMode();
}

/**
 * Handle gear.
 */
Deadeye::State Deadeye::DoGear() {
  SPDLOG_TRACE(logger_, "entering State::gear");
  return CheckMode();
}

/**
 * Handle error.
 */
Deadeye::State Deadeye::DoError() {
  SPDLOG_TRACE(logger_, "entering State::error");
  if (!error_reported_) {
    DriverStation::ReportError("ROBOT VISION SYSTEM IS UNAVAILABLE");
    error_reported_ = true;
  }
  return State::error;
}

/**
 * Send a sentence to Deadeye.
 */
void Deadeye::SendSentence(const Sentence& sentence) {
  SPDLOG_DEBUG(logger_, "sending {}", sentence.ToString());
  // TODO: check returned number of bytes written
  try {
    serial_->write(sentence.ToString() + "\n");
  } catch (const std::exception& e) {
    logger_->critical(e.what());
  }
}

/**
 * Read a sentence from Deadeye.
 */
Deadeye::State Deadeye::ReadSentence() {
  std::string line;
  try {
    line = serial_->readline();
  } catch (const std::exception& e) {
    logger_->critical(e.what());
    return State::error;
  }

  if (line.size() == 0) {
    return state_;
  }
  line.pop_back();  // remove trailing newline
  parser_.ParseText(sentence_, line);
  bool valid = sentence_.Valid();
  SPDLOG_TRACE(logger_, "read {} sentence: \"{}\"", valid ? "VALID" : "INVALID",
               line);
  return state_;
}

/**
 * Get the azimuth error.
 */
int Deadeye::GetAzimuthError() {
  std::lock_guard<std::mutex> lock(mutex_);
  return azimuth_error_;
}

/**
 * Get the azimuth error.
 */
int Deadeye::GetCenterlineError() {
  std::lock_guard<std::mutex> lock(mutex_);
  return centerline_error_;
}

/**
 * SetGearLightEnabled
 */
void Deadeye::SetGearLightEnabled(bool enable) {
  RobotMap::gear_camera_led->Set(!enable);
}

/**
 * SetShooterLightEnabled
 */
void Deadeye::SetShooterLightEnabled(bool enable) {
  RobotMap::shooter_camera_led->Set(!enable);
}

/**
 * Load settings from global config.
 */
void Deadeye::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("DEADEYE");
  if (!config) {
    throw invalid_argument("STEAMWORKS DEADEYE table not present");
  }

  const char* missing = "STEAMWORKS DEADEYE {} setting missing, using default";

  auto s_opt = config->get_as<string>("port");
  if (s_opt) {
    port_ = *s_opt;
  } else {
    logger_->warn(missing, "port");
  }
  logger_->info("deadeye serial port: {}", port_);

  auto u_opt = config->get_as<uint32_t>("port_speed");
  if (u_opt) {
    speed_ = *u_opt;
  } else {
    logger_->warn(missing, "port_speed");
  }
  logger_->info("deadeye serial port speed: {}", speed_);

  auto i_opt = config->get_as<int>("period");
  if (i_opt) {
    period_ = *i_opt;
  } else {
    logger_->warn(missing, "period");
  }
  logger_->info("deadeye notification period: {} ms", period_);
}
