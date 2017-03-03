#include "deadeye.h"

#include <cpptoml/cpptoml.h>
#include <serial/serial.h>

#include "parser.h"
#include "sentence.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace std;

Deadeye::Deadeye(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Deadeye"),
      logger_(spdlog::get("subsystem")),
      thread_{},
      stop_thread_{false} {
  LoadConfigSettings(config);
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
  serial::Timeout timeout{0, 50, 0, 0, 0};
  serial::Serial serial{port_, speed_, timeout};
  serial.flushInput();
  deadeye::Parser parser;
  deadeye::Sentence sentence;
  while (!stop_thread_) {
    // QUESTION: will no read timeout block the destructor join?
    std::string line = serial.readline();
    parser.ParseText(sentence, line);
    bool valid = sentence.Valid();
    // logger->trace(
    //     "name = {}, valid = {}, parameters[0] = {}, parameters[1] = {}",
    //     sentence.name, valid, sentence.parameters[0],
    //     sentence.parameters[1]);
    logger->trace("name = {}, valid = {}", sentence.name, valid);
    if (!valid) {
      logger->warn("sentence invalid: {}", line);
      continue;
    }
    // int azimuth_error = std::stoi(sentence.parameters[0]);
    // int range = std::stoi(sentence.parameters[1]);
    // {
    //   std::lock_guard<std::mutex> lock(mutex_);
    //   azimuth_error_ = azimuth_error;
    //   range_ = range;
    // }
  }
  serial.close();
}

/**
 * SetGearLightEnabled
 */
void Deadeye::SetGearLightEnabled(bool enable) {
  RobotMap::gear_camera_led->Set(!enable);
}

/**
 * Load settings from global config.
 */
void Deadeye::LoadConfigSettings(const std::shared_ptr<cpptoml::table> config) {
  auto camera_config = config->get_table("STEAMWORKS")->get_table("CAMERA");
  if (!camera_config) {
    // throw invalid_argument("STEAMWORKS CAMERA table not present");
  }

  const char* missing = "STEAMWORKS CAMERA {} setting missing, using default";

  auto s_opt = camera_config->get_as<string>("port");
  if (s_opt) {
    port_ = *s_opt;
  } else {
    logger_->warn(missing, "port");
  }
  logger_->info("camera serial port: {}", port_);

  auto u_opt = camera_config->get_as<uint32_t>("port_speed");
  if (u_opt) {
    speed_ = *u_opt;
  } else {
    logger_->warn(missing, "port_speed");
  }
  logger_->info("camera serial port speed: {}", port_);
}
