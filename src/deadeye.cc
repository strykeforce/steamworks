#include "deadeye.h"

#include <chrono>
#include <thread>

using namespace deadeye;

namespace {
constexpr auto NTGT_SLEEP_MS = std::chrono::milliseconds(10);
}

Deadeye::Deadeye(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")),
      link_(config),
      boiler_camera_(config),
      gear_camera_(config) {
  LoadConfigSettings(config);
  link_.Start();
}

/**
 * Main loop for camera frame aquistion and processing.
 */
void Deadeye::Run() {
  while (true) {
    int mode = link_.GetMode();

    if (mode != current_mode_) {
      SwitchMode(mode);
      current_mode_ = mode;
    }

    switch (mode) {
      case Link::Mode::boiler:
        ProcessBoilerTarget();
        break;
      case Link::Mode::gear:
        ProcessGearTarget();
        break;
      case Link::Mode::idle:
        std::this_thread::sleep_for(NTGT_SLEEP_MS);
        continue;
    }
  }
}

/**
 * Switch active mode.
 */
void Deadeye::SwitchMode(int mode) {
  switch (mode) {
    case Link::Mode::boiler:
      logger_->info("Deadeye switching to boiler camera capture");
      SPDLOG_TRACE(logger_, "start StopCapture");
      if (!boiler_camera_.IsConnected()) {
        SPDLOG_TRACE(logger_, "start Connect");
        boiler_camera_.Connect();
      }
      SPDLOG_TRACE(logger_, "start StartCapture");
      boiler_camera_.StartCapture();
      SPDLOG_TRACE(logger_, "done configuring camera");
      break;
    case Link::Mode::gear:
      logger_->info("Deadeye switching to gear camera capture");
      boiler_camera_.StopCapture();
      if (!gear_camera_.IsConnected()) {
        gear_camera_.Connect();
      }
      break;
    case Link::Mode::idle:
      logger_->info("deadeye mode set to idle");
      boiler_camera_.StopCapture();
      break;
    default:
      logger_->info("EnableCamera called with unknown mode");
  }
}

/**
 * Called in boiler mode after frame acquired.
 */
void Deadeye::ProcessBoilerTarget() {
#ifdef LOG_FPS
  static int framerate_count = 0;
  if (framerate_count == 0) {
    fps_.Start();
  }
  if (framerate_count++ > display_framerate_int_) {
    fps_.Stop();
    logger_->info("FPS = {}", fps_.FramesPerSecond());
    framerate_count = 0;
  }
  fps_.Update();
#endif

  int centerline_error;  // vertical target separation
  int azimuth_error;
  bool success = boiler_camera_.ProcessFrame(azimuth_error, centerline_error);

#ifdef DISPLAY_FRAME
  boiler_camera_.DisplayFrame();
#endif

#ifdef LOG_BOILER
  static int boiler_count = 0;
  if (boiler_count++ > display_boiler_int_) {
    logger_->info("boiler acquired = {}, azimuth = {}, centerline = {}",
                  success, azimuth_error, centerline_error);
    boiler_count = 0;
  }
#endif

  if (success) {
    link_.SendBoilerSolution(azimuth_error, centerline_error);
    return;
  }
  // logger_->warn("boiler targets not visible");
  link_.SendNoTarget();
  std::this_thread::sleep_for(NTGT_SLEEP_MS);
}

void Deadeye::ProcessGearTarget() {
#ifdef LOG_FPS
  static int framerate_count = 0;
  if (framerate_count == 0) {
    fps_.Start();
  }
  if (framerate_count++ > display_framerate_int_) {
    fps_.Stop();
    logger_->info("FPS = {}", fps_.FramesPerSecond());
    framerate_count = 0;
  }
  fps_.Update();
#endif

  int azimuth_error;
  int target_height;
  bool success = gear_camera_.ProcessFrame(azimuth_error, target_height);

#ifdef DISPLAY_FRAME
  gear_camera_.DisplayFrame();
#endif

#ifdef LOG_GEAR
  static int gear_count = 0;
  if (gear_count++ > display_gear_int_) {
    logger_->info("gear acquired = {}, azimuth = {}, target height = {}",
                  success, azimuth_error, target_height);
    gear_count = 0;
  }
#endif

  if (success) {
    link_.SendGearSolution(azimuth_error, target_height);
    return;
  }
  link_.SendNoTarget();
  std::this_thread::sleep_for(NTGT_SLEEP_MS);
}

void Deadeye::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  assert(config_in);
  auto config = config_in->get_table("DEADEYE")->get_table("DEBUG");

#ifdef LOG_FPS
  auto framerate_opt = config->get_as<int>("framerate");
  if (framerate_opt) {
    display_framerate_int_ = *framerate_opt;
  }
  logger_->warn("logging framerate every {} frames", display_framerate_int_);
#else
  logger_->info("framerate logging disabled");
#endif

#ifdef LOG_BOILER
  auto boiler_opt = config->get_as<int>("boiler");
  if (boiler_opt) {
    display_boiler_int_ = *boiler_opt;
  }
  logger_->warn("logging boiler solution every {} frames", display_boiler_int_);
#else
  logger_->info("boiler solution logging disabled");
#endif

#ifdef LOG_GEAR
  auto gear_opt = config->get_as<int>("gear");
  if (gear_opt) {
    display_gear_int_ = *gear_opt;
  }
  logger_->warn("logging gear solution every {} frames", display_gear_int_);
#else
  logger_->info("gear solution logging disabled");
#endif
}
