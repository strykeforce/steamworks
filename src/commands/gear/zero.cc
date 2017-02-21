#include "zero.h"

#include "robot.h"

using namespace steamworks::command::gear;

namespace {
const int kStableCountReq = 3;
}

ZeroPivot::ZeroPivot() : frc::Command("ZeroPivot") {
  Requires(Robot::gear_loader);
}

/**
 * Initialize pushes against zero.
 */
void ZeroPivot::Initialize() {
  Robot::gear_loader->SetPivotZeroModeEnabled(true);
  stable_count_ = 0;
}

/**
 * IsFinished returns after 3 stable encoder readings.
 */
bool ZeroPivot::IsFinished() {
  int encoder = Robot::gear_loader->GetPivotPosition();
  if (encoder == last_encoder_) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    return true;
  }
  last_encoder_ = encoder;
  return false;
}

void ZeroPivot::End() {
  Robot::gear_loader->SetPivotEncoderZero();
  Robot::gear_loader->SetPivotZeroModeEnabled(false);
}
