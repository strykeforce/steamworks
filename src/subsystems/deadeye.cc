#include "deadeye.h"

#include "robot_map.h"
#include "shooter_target_data.h"

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
  // SPDLOG_TRACE(logger_, "entering State::idle");
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
  // SPDLOG_TRACE(logger_, "entering State::boiler");
  if (ReadSentence() == Deadeye::State::error) {
    return Deadeye::State::error;
  }
  if (sentence_.Valid()) {
    switch (sentence_.Code()) {
      case Sentence::kBoiler:
        azimuth_error_ = stoi(sentence_.parameters[1]);
        centerline_error_ = stoi(sentence_.parameters[2]);
        has_target_ = true;
        break;
      case Sentence::kNoTarget:
        SPDLOG_TRACE(logger_, "no target");
        has_target_ = false;
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
  SPDLOG_TRACE(logger_, "read {} sentence: \"{}\"",
               sentence_.Valid() ? "VALID" : "INVALID", line);
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
 * True if the last communication from deadeye was a valid target.
 */
bool Deadeye::HasTarget() {
  std::lock_guard<std::mutex> lock(mutex_);
  return has_target_;
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
 * Calculates shooter solution based on given input, returns true if valid
 * solution found.
 */
bool Deadeye::CalculateSolution(int centerline_elevation) {
  // logger_->error("Deadeye testing failed shooter solution");
  // return false;

  double shooter_angle =
      elevation_zero_angle_ + (centerline_elevation * degrees_per_tick_);
  logger_->debug("Deadeye shooter angle = {}", shooter_angle);

  double aiming_angle = (camera_angle_ - shooter_angle) * M_PI / 180.0;
  logger_->debug("Deadeye aiming angle = {} deg,  {} rad",
                 camera_angle_ - shooter_angle, aiming_angle);

  solution_range_ =
      (centerline_height_ - camera_height_) / std::tan(aiming_angle);

  solution_range_ += 10;  // front of target to center of boiler
  logger_->debug("Deadeye solution range = {}", solution_range_);

  int range_lookup =
      static_cast<int>(std::round(solution_range_)) - shooter_range_offset;
  if (range_lookup < 0 || range_lookup > shooter_data_size) {
    logger_->error(
        "Deadeye range lookup value out of range, solution range = {}",
        solution_range_);
    return false;
  }
  solution_elevation_ = shooter_data[range_lookup][kElevation];
  solution_wheel_speed_ = shooter_data[range_lookup][kSpeed];
  solution_azimuth_offset_ = shooter_data[range_lookup][kAzimuth];

  logger_->info(
      "Deadeye solution: range = {}, elevation = {}, speed = {}, "
      "azimuth "
      "offset = {}",
      solution_range_, solution_elevation_, solution_wheel_speed_,
      solution_azimuth_offset_);
  return true;
}

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Deadeye::GetSolutionRange() { return solution_range_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Deadeye::GetSolutionElevation() { return solution_elevation_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Deadeye::GetSolutionWheelSpeed() { return solution_wheel_speed_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Deadeye::GetSolutionAzimuthOffset() { return solution_azimuth_offset_; }

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

  auto d_opt = config->get_as<double>("elevation_zero_angle");
  if (d_opt) {
    elevation_zero_angle_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER elevation_zero_angle setting not available, using "
        "default");
  }
  logger_->info("shooter elevation zero angle: {}", elevation_zero_angle_);
  d_opt = config->get_as<double>("centerline_height");
  if (d_opt) {
    centerline_height_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER centerline_height setting not available, using "
        "default");
  }
  logger_->info("shooter target centerline height: {}", centerline_height_);
  d_opt = config->get_as<double>("camera_height");
  if (d_opt) {
    camera_height_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER camera_height setting not available, using "
        "default");
  }
  logger_->info("shooter camera height: {}", camera_height_);
  d_opt = config->get_as<double>("camera_angle");
  if (d_opt) {
    camera_angle_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER camera_angle setting not available, using "
        "default");
  }
  logger_->info("shooter camera angle: {}", camera_angle_);
  d_opt = config->get_as<double>("degrees_per_tick");
  if (d_opt) {
    degrees_per_tick_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER degrees_per_tick setting not available, using "
        "default");
  }
  logger_->info("shooter degrees per tick: {}", degrees_per_tick_);
}
