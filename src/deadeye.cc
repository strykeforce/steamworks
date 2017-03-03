#include "deadeye.h"

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "boiler_target_data.h"
#include "link/mode.h"

using namespace deadeye;

Deadeye::Deadeye(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")), link_(config), boiler_camera_(config) {
  boiler_camera_.Connect();
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
      case Mode::quit:
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
      logger_->info("starting boiler camera capture");
      boiler_camera_.StartCapture();
      break;
    case Mode::gear:
      logger_->info("starting gear camera capture");
      break;
    case Mode::quit:
      logger_->info("deadeye mode set to quit");
      break;
    default:
      logger_->info("EnableCamera called with unknown mode");
  }
}

/**
 * Called in boiler mode after frame acquired.
 */
void Deadeye::ProcessBoilerTarget() {
  int y;  // vertical target separation
  int azimuth_error;
  std::tie(azimuth_error, y) = boiler_camera_.ProcessFrame();
#if !NDEBUG
  boiler_camera_.DisplayFrame();
#endif
  y -= boiler_target_offset;
  if (y < 0 || y > boiler_target_data_size - 1) {
    logger_->warn("boiler target separation distance out of range: {} px", y);
    link_.SendNoTarget();
    return;
  }
  link_.SendBoilerSolution(azimuth_error, boiler_target_data[y][kRange],
                           boiler_target_data[y][kAngle],
                           boiler_target_data[y][kSpeed]);

  // TODO: config file should decide
}

void Deadeye::ProcessGearTarget() {
  logger_->error("{} not implemented", __FUNCTION__);
}
