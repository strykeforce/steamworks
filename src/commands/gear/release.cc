#include "release.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::command::gear;

namespace {
const double kCloseEnough = 3;
const int kStableCountReq = 3;
}

ReleaseGear::ReleaseGear()
    : frc::Command("ReleaseGear"), logger_(spdlog::get("command")) {
  Requires(Robot::gear_loader);
}

/**
* Release gear on lift peg.
*/
void ReleaseGear::Initialize() {
  Robot::gear_loader->ClampRelease();
  Robot::gear_loader->SetPivotPosition(-800);
  stable_count_ = 0;
}

/**
 * Finished when pivot has returned to lowered position.
 */
bool ReleaseGear::IsFinished() {
  int pos = Robot::gear_loader->GetPivotPosition();

  SPDLOG_DEBUG(logger_, "ReleaseGear pivot motor pos = ", pos);

  if (fabs(pivot_pos_ - pos) <= kCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    return true;
  }
  pivot_pos_ = pos;
  return false;
}

/**
 * Stow clamp after releasing.
 */
void ReleaseGear::End() {
  Robot::gear_loader->SetPivotEncoderZero();
  logger_->info("ReleaseGear ended with pos {}, abs. pos {}",
                Robot::gear_loader->GetPivotPosition(),
                Robot::gear_loader->GetPivotAbsPosition());
  Robot::gear_loader->ClampShut();
  Robot::gear_loader->SetPivotPosition(0);
}
