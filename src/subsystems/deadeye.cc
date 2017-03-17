#include "deadeye.h"

#include <sys/socket.h>
#include <msgpack.hpp>

#include "robot_map.h"
#include "shooter_target_data.h"

using namespace steamworks::subsystem;
using namespace std;

// TODO: look at Notify to run this loop; should reduce CPU load
Deadeye::Deadeye(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Deadeye"), logger_(spdlog::get("deadeye")) {
  LoadConfigSettings(config);
  ConfigureNetworking();

  // turn off camera LEDs at robot start-up
  RobotMap::gear_camera_led->Set(true);
  RobotMap::shooter_camera_led->Set(true);
}

/**
 * Stop the IO notification loop.
 */
Deadeye::~Deadeye() {}

/**
 * Start the IO thread, runs forever.
 */
void Deadeye::Start() {
  thread_ = std::thread{&Deadeye::Run, this};
  thread_.detach();
}

/**
 * The main IO loop.
 */
void Deadeye::Run() {
  char recv[256];
  for (;;) {
    auto nread = recvfrom(sockfd_, recv, sizeof(recv), 0, NULL, 0);
    if (nread == -1) {
      logger_->error("Deadeye recvfrom: {}", strerror(errno));
    }
    auto oh = msgpack::unpack(recv, nread);
    auto obj = oh.get();
    array<int, 3> message;
    obj.convert(message);
    // SPDLOG_DEBUG(logger_, "Deadeye recvfrom: [{},{},{}]", message[0],
    //              message[1], message[2]);
    {
      std::lock_guard<std::mutex> lock(mutex_);
      switch (message[0]) {
        case kNoTargetMesg:
          has_target_ = false;
          break;
        case kBoilerSolutionMesg:
          has_target_ = true;
          azimuth_error_ = message[1];
          centerline_error_ = message[2];
          break;
        default:
          has_target_ = false;
          break;
      }
    }
  }
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
 * Configure UDP networking to roboRIO.
 */
void Deadeye::ConfigureNetworking() {
  sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ == -1) {
    logger_->critical("Link socket error: {}", strerror(errno));
  }

  // listening address
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(5800);

  if (::bind(sockfd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
    logger_->critical("Link bind error: {}", strerror(errno));
  }
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

  auto i_opt = config->get_as<int>("port");
  if (i_opt) {
    port_ = *i_opt;
  } else {
    logger_->warn(missing, "port");
  }
  logger_->info("deadeye port: {}", port_);

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