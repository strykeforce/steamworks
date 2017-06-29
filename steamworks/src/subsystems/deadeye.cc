#include "deadeye.h"

#ifdef LOG_FPS
#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif
#include <iomanip>
#include "log.h"
#endif

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
Deadeye::~Deadeye() {
#ifdef LOG_FPS
  EndTelemetry();
#endif
}

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
#ifdef LOG_FPS
  InitializeTelemetry();
#endif
  char buf[256];
  for (;;) {
    // auto nread = recvfrom(recvfd_, recv, sizeof(recv), 0, NULL, NULL);
    auto nread = recvfrom(recvfd_, buf, sizeof(buf), 0, NULL, NULL);
    if (nread == -1) {
      logger_->error("Deadeye recvfrom: {}", strerror(errno));
    }
    auto oh = msgpack::unpack(buf, nread);
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
        case kGearSolutionMesg:
          has_target_ = true;
          azimuth_error_ = message[1];
          target_height_ = message[2];
          break;
        case kErrorMesg:
          logger_->error("Deadeye hardware error");
          break;
        default:
          has_target_ = false;
          break;
      }
    }
#ifdef LOG_FPS
    LogTelemetry();
#endif
  }
}

/**
 * Get the azimuth error.
 */
int Deadeye::GetAzimuthError() {
  std::lock_guard<std::mutex> lock(mutex_);
  return azimuth_error_ + azimuth_correction_;
}

/**
 * Get the strafe error.
 */
int Deadeye::GetStrafeError() {
  std::lock_guard<std::mutex> lock(mutex_);
  return azimuth_error_;
}

/**
 * Get the centerline error.
 */
int Deadeye::GetCenterlineError() {
  std::lock_guard<std::mutex> lock(mutex_);
  return centerline_error_;
}

/**
 * Get the azimuth error.
 */
int Deadeye::GetTargetHeight() {
  std::lock_guard<std::mutex> lock(mutex_);
  return target_height_;
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
 * Switch to boiler camera.
 */
void Deadeye::EnableBoilerCamera() {
  logger_->info("Deadeye enabling boiler camera");
  array<int, 3> t{{kBoilerSolutionMesg, 0, 0}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (sendto(sendfd_, buf.data(), nwrite, 0, (sockaddr*)&remote_addr_,
             sizeof(remote_addr_)) != nwrite) {
    logger_->warn("Deadeye error sending message: {}", strerror(errno));
  }
}

/**
 * Switch to gear camera.
 */
void Deadeye::EnableGearCamera() {
  logger_->info("Deadeye enabling gear camera");
  array<int, 3> t{{kGearSolutionMesg, 0, 0}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (sendto(sendfd_, buf.data(), nwrite, 0, (sockaddr*)&remote_addr_,
             sizeof(remote_addr_)) != nwrite) {
    logger_->warn("Deadeye error sending message: {}", strerror(errno));
  }
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
  logger_->debug("Deadeye uncorrected solution range = {}", solution_range_);

  if (solution_range_ >= 168.0) {
    // apply a linearly-increasing correction from 168 -> 240 in (current
    // maximum range) for empirically observed range error
    solution_range_ += 3 * (1 - ((240 - solution_range_) / (240.0 - 168.0)));
  }

  int range_lookup =
      static_cast<int>(std::round(solution_range_)) - shooter_range_offset;
  if (range_lookup < 0 || range_lookup > shooter_data_size) {
    logger_->error(
        "Deadeye range lookup value out of range, solution range = {}",
        solution_range_);
    return false;
  }
  solution_elevation_ = shooter_data[range_lookup][kElevation];
  solution_hopper_voltage_ = shooter_data[range_lookup][kHopper];
  solution_azimuth_offset_ = shooter_data[range_lookup][kAzimuth];

  auto range_delta =
      solution_range_in_delta_ * shooter_data[range_lookup][kInDelta];
  solution_wheel_speed_ = shooter_data[range_lookup][kSpeed] + range_delta;
  logger_->info(
      "Deadeye solution: range = {}, elevation = {}, speed = {}, "
      "hopper = {} "
      "range speed delta = {}",
      solution_range_, solution_elevation_, solution_wheel_speed_,
      solution_hopper_voltage_, range_delta);
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
double Deadeye::GetSolutionHopperVoltage() { return solution_hopper_voltage_; }

/**
 * Calculate the shooter solution based on camera angle and pixel distance from
 * targets centerline.
 */
double Deadeye::GetSolutionAzimuthOffset() { return solution_azimuth_offset_; }

/**
 * Configure UDP networking to roboRIO.
 */
void Deadeye::ConfigureNetworking() {
  recvfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (recvfd_ == -1) {
    logger_->critical("Link socket error: {}", strerror(errno));
  }

  // listening address
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(recv_port_);

  if (::bind(recvfd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
    logger_->critical("Link bind error: {}", strerror(errno));
  }

  // sending to deadeye remote
  sendfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sendfd_ == -1) {
    logger_->critical("Link socket error: {}", strerror(errno));
  }

  // remote host with "address" and "port" in LINK config table
  memset(&remote_addr_, 0, sizeof(remote_addr_));
  remote_addr_.sin_family = AF_INET;
  if (inet_pton(AF_INET, remote_.c_str(), &remote_addr_.sin_addr) != 1) {
    logger_->critical("Deadeye inet_pton error for address {}", remote_);
  }
  remote_addr_.sin_port = htons(send_port_);
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

  auto i_opt = config->get_as<int>("recv_port");
  if (i_opt) {
    recv_port_ = *i_opt;
  } else {
    logger_->warn(missing, "recv_port");
  }
  logger_->info("deadeye recv_port: {}", recv_port_);

  i_opt = config->get_as<int>("send_port");
  if (i_opt) {
    send_port_ = *i_opt;
  } else {
    logger_->warn(missing, "send_port");
  }
  logger_->info("deadeye send_port: {}", send_port_);

  auto s_opt = config->get_as<string>("remote");
  if (s_opt) {
    remote_ = *s_opt;
  } else {
    logger_->warn(missing, "remote");
  }
  logger_->info("deadeye remote: {}", remote_);

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

  i_opt = config->get_as<int>("azimuth_correction");
  if (i_opt) {
    azimuth_correction_ = *i_opt;
  } else {
    logger_->warn(missing, "azimuth_correction");
  }
  logger_->info("deadeye azimuth_correction: {}", azimuth_correction_);

  d_opt = config->get_as<double>("range_in_delta");
  if (d_opt) {
    solution_range_in_delta_ = *d_opt;
  } else {
    logger_->error(
        "STEAMWORKS.SHOOTER range_in_delta setting not available, "
        "using "
        "default");
  }
  logger_->info("shooter range delta (in): {}", solution_range_in_delta_);
}

#ifdef LOG_FPS
namespace {
const string kTelemetryPath = "/home/lvuser/logs/deadeye_fps_";
}

/**
 * Open log file for telemetry.
 */
void Deadeye::InitializeTelemetry() {
  string path = Log::GetTelemetryFilePath(kTelemetryPath);
  logger_->info("Deadeye logging telemetry to {}", path);
  telemetry_ = make_unique<ofstream>(path, ofstream::trunc);
  *telemetry_
      << "timestamp,has_target,frame_period,azimuth_error,centerline_error\n";
  last_frame_ = telemetry_start_ = timer_.GetFPGATimestamp();
}

/**
 * Log a line of telemetry.
 */
void Deadeye::LogTelemetry() {
  double ts = timer_.GetFPGATimestamp();
  *telemetry_ << setprecision(0) << fixed << (ts - telemetry_start_) * 1000
              << "," << has_target_ * 10 << "," << (ts - last_frame_) * 1000
              << setprecision(2) << "," << azimuth_error_ << ","
              << centerline_error_ << "\n";
  last_frame_ = ts;
}

/**
 * Close telemetry log file.
 */
void Deadeye::EndTelemetry() { telemetry_->close(); }

#endif
