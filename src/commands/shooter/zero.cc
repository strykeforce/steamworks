#include "zero.h"

#include "robot.h"

using namespace steamworks::command::shooter;

namespace {
const double kDriveVolts = -1.5;
const double kSetVolts = -1.5;
const int kStableCountReq = 3;
}

//
// PositionElevation
//
PositionElevationZero::PositionElevationZero()
    : frc::Command("PositionElevationZero") {
  Requires(Robot::shooter);
}

/**
 * Initialize moves the elevation to the limit switch in preparation.
 */
void PositionElevationZero::Initialize() {
  Robot::shooter->SetElevationZeroModeEnabled(true);
  Robot::shooter->SetElevationZeroDriveVoltage(kDriveVolts);
}

/**
 * IsFinished
 */
bool PositionElevationZero::IsFinished() {
  return Robot::shooter->IsElevationZeroLimitSwitchActive();
}

//
// ZeroElevation
//
ZeroElevation::ZeroElevation() : frc::Command("ZeroElevation") {
  Requires(Robot::shooter);
}

/**
 * Initialize pushes against zero.
 */
void ZeroElevation::Initialize() {
  Robot::shooter->SetElevationZeroDriveVoltage(kSetVolts);
  stable_count_ = 0;
}

/**
 * IsFinished returns after 3 stable encoder readings.
 */
bool ZeroElevation::IsFinished() {
  unsigned encoder = Robot::shooter->GetElevation();
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

//
// SetElevationZero
//
SetElevationZero::SetElevationZero() : frc::InstantCommand("SetElevationZero") {
  Requires(Robot::shooter);
}

/**
 * Initialize sets encoder to zero.
 */
void SetElevationZero::Initialize() {
  Robot::shooter->SetElevationEncoderZero();
}

void SetElevationZero::End() {
  Robot::shooter->SetElevationZeroModeEnabled(false);
}
