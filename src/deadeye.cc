#include "deadeye.h"

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "boiler_target_data.h"

using namespace deadeye;

Deadeye::Deadeye(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")), link_(config), boiler_camera_(config) {
  boiler_camera_.Connect();
}

void Deadeye::Run() {
  boiler_camera_.StartCapture();
  while (true) {
    // TODO: start/stop capture when mode changes
    switch (link_.GetMode()) {
      case Link::kBoilerMode:
        ProcessBoilerTarget();
        break;
      case Link::kGearMode:
        ProcessGearTarget();
        break;
      case Link::kQuitMode:
        return;
    }
  }
}

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
