#include "deadeye.h"

#include <chrono>
#include <thread>

#include "link/mode.h"

using namespace deadeye;

namespace {
constexpr auto NTGT_SLEEP_MS = std::chrono::milliseconds(50);
}

Deadeye::Deadeye(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")),
      link_(config),
      boiler_camera_(config),
      gear_camera_(config) {
  LoadConfigSettings(config);
}

/**
 * Main loop for camera frame aquistion and processing.
 */
void Deadeye::Run() {
  while (true) {
    Mode mode = link_.GetMode();

    if (mode != current_mode_) {
      SwitchMode(mode);
      current_mode_ = mode;
    }

    switch (mode) {
      case Mode::boiler:
        ProcessBoilerTarget();
        break;
      case Mode::gear:
        ProcessGearTarget();
        break;
      case Mode::idle:
        return;
    }
  }
}

/**
 * Switch active mode.
 */
void Deadeye::SwitchMode(Mode mode) {
  switch (mode) {
    case Mode::boiler:
      logger_->info("switching to boiler camera capture");
      SPDLOG_TRACE(logger_, "start StopCapture");
      gear_camera_.StopCapture();
      if (!boiler_camera_.IsConnected()) {
        SPDLOG_TRACE(logger_, "start Connect");
        boiler_camera_.Connect();
      }
      SPDLOG_TRACE(logger_, "start StartCapture");
      boiler_camera_.StartCapture();
      SPDLOG_TRACE(logger_, "done configuring camera");
      break;
    case Mode::gear:
      logger_->info("switching to gear camera capture");
      boiler_camera_.StopCapture();
      if (!gear_camera_.IsConnected()) {
        gear_camera_.Connect();
      }
      gear_camera_.StartCapture();
      break;
    case Mode::idle:
      logger_->info("deadeye mode set to idle");
      gear_camera_.StopCapture();
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
  if (display_framerate_) {
    static int framerate_count = 0;
    if (framerate_count == 0) {
      fps_.Start();
    }
    if (framerate_count++ > display_framerate_int_) {
      fps_.Stop();
      logger_->info("FPS = {}", fps_.FramesPerSecond());
      framerate_count = 0;
    }
  }
  fps_.Update();
  int centerline_error;  // vertical target separation
  int azimuth_error;
  bool success = boiler_camera_.ProcessFrame(azimuth_error, centerline_error);
  boiler_camera_.DisplayFrame();
  if (display_boiler_) {
    static int boiler_count = 0;
    if (boiler_count++ > display_boiler_int_) {
      logger_->info("boiler acquired = {}, azimuth = {}, centerline = {}",
                    success, azimuth_error, centerline_error);
      boiler_count = 0;
    }
  }
  if (success) {
    link_.SendBoilerSolution(azimuth_error, centerline_error);
    return;
  }
  // logger_->warn("boiler targets not visible");
  link_.SendNoTarget();
  std::this_thread::sleep_for(NTGT_SLEEP_MS);
}

void Deadeye::ProcessGearTarget() {
  int azimuth_error;
  int target_width;
  bool success = gear_camera_.ProcessFrame(azimuth_error, target_width);
  int range = azimuth_error + target_width;  // FIXME: NOPE!
  gear_camera_.DisplayFrame();
  if (success) {
    link_.SendGearSolution(azimuth_error, range);
    return;
  }
  link_.SendNoTarget();
  std::this_thread::sleep_for(NTGT_SLEEP_MS);
}

void Deadeye::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  assert(config_in);
  auto config = config_in->get_table("DEADEYE")->get_table("DEBUG");
  auto i_opt = config->get_as<int>("framerate");
  if (i_opt) {
    display_framerate_int_ = *i_opt;
  }
  display_framerate_ = display_framerate_int_ > 0;
  logger_->info("displaying framerate: {} every {} frames", display_framerate_,
                display_framerate_int_);

  i_opt = config->get_as<int>("boiler");
  if (i_opt) {
    display_boiler_int_ = *i_opt;
  }
  display_boiler_ = display_boiler_int_ > 0;
  logger_->info("displaying boiler solution: {} every {} frames",
                display_boiler_, display_boiler_int_);

  i_opt = config->get_as<int>("gear");
  if (i_opt) {
    display_gear_int_ = *i_opt;
  }
  display_gear_ = display_gear_int_ > 0;
  logger_->info("displaying gear solution: {} every {} frames", display_gear_,
                display_gear_int_);
}
