#include "deadeye.h"

#include <serial/serial.h>
#include <chrono>
#include <thread>

#include "parser.h"
#include "sentence.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace deadeye;
using namespace std;

namespace {
constexpr auto LOOP_SLEEP_MS = std::chrono::milliseconds(20);
}

// TODO: look at Notify to run this loop; should reduce CPU load
Deadeye::Deadeye(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Deadeye"),
      logger_(spdlog::get("subsystem")),
      thread_{},
      stop_thread_{false} {
  LoadConfigSettings(config);
  // turn off camera LEDs at robot start-up
  RobotMap::gear_camera_led->Set(true);
  RobotMap::shooter_camera_led->Set(true);
}

/**
 * Stop the IO thread.
 */
Deadeye::~Deadeye() {
  stop_thread_ = true;
  if (thread_.joinable()) thread_.join();
}

/**
 * Start the IO thread.
 */
void Deadeye::Start() { thread_ = std::thread{&Deadeye::Run, this}; }

/**
 * IO thread loop.
 */
void Deadeye::Run() {
  auto logger = spdlog::get("deadeye");
  serial::Timeout timeout{0, 100, 0, 0, 0};
  serial::Serial serial{port_, speed_, timeout};
  serial.flushInput();
  Parser parser;
  Sentence sentence;
  Mode current_mode_ = Mode::quit;
  while (!stop_thread_) {
    // QUESTION: will no read timeout block the destructor join?
    std::this_thread::sleep_for(LOOP_SLEEP_MS);
    if (current_mode_ != mode_) {
      ModeSentence mode(mode_);
      serial.write(mode.ToString() + "\n");
      current_mode_ = mode_;
      continue;
    }
    std::string line = serial.readline();
    parser.ParseText(sentence, line);
    bool valid = sentence.Valid();
    if (!valid) {
      //   if (!error_reported_) {
      //     logger->error("sentence invalid: {}", line);
      //     error_reported_ = true;
      //   }
      continue;
    }
    // error_reported_ = false;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      switch (sentence.Code()) {
        case 0:
          // no target
          break;
        case 1:  // boiler target
          azimuth_error_ = stoi(sentence.parameters[1]);
          centerline_error_ = stoi(sentence.parameters[2]);
          break;
        case 2:  // gear target
          azimuth_error_ = stoi(sentence.parameters[1]);
          range_ = stoi(sentence.parameters[2]);
          SPDLOG_TRACE(logger_, "gear azimuth error = {} range = {}",
                       azimuth_error_, centerline_error_);
          break;
        case 3:  // mode
          break;
        default:
          throw logic_error("unrecognized sentence code");
      }
    }
  }
  serial.close();
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
 * Get the current Deadeye mode.
 */
Mode Deadeye::GetMode() {
  std::lock_guard<std::mutex> lock(mutex_);
  return mode_;
}

/**
 * Set the current Deadeye mode.
 */
// void Deadeye::SetMode(Mode mode) {}

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
  logger_->info("camera serial port: {}", port_);

  auto u_opt = config->get_as<uint32_t>("port_speed");
  if (u_opt) {
    speed_ = *u_opt;
  } else {
    logger_->warn(missing, "port_speed");
  }
  logger_->info("camera serial port speed: {}", speed_);
}
