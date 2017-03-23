#include "place_gear.h"

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"

using namespace steamworks::command::gear;
using namespace std;

namespace {
const double kSetpointMax = 275.0;

const double kStrafeMinSpeed = 15.0 / kSetpointMax;
const double kStrafeMaxSpeed = 50.0 / kSetpointMax;
const double kStrafeSlopeStart = 60;
const double kStrafeCloseEnough = 3;
const double kStrafeDeadzone = 0.1 * kStrafeMinSpeed;

const int kStableCountReq = 3;
}

PlaceGear::PlaceGear()
    : frc::Command("PlaceGear"), logger_(spdlog::get("command")) {
  Requires(Robot::drive);
}

/**
 * Initialize
 */
void PlaceGear::Initialize() {
  Robot::drive->SetAzimuthMode();
  strafe_error_ = Robot::deadeye->GetAzimuthError();
  logger_->info("PlaceGear initialized with strafe error {}", strafe_error_);
  stable_count_ = 0;
}

/**
 * Execute the main loop
 */
void PlaceGear::Execute() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->warn("PlaceGear::Execute no target");
    return;
  }
  // strafe_error_ = Robot::deadeye->GetAzimuthError();
  strafe_error_ = Robot::deadeye->GetAzimuthError();
  strafe_abs_error_ = fabs(strafe_error_);
  double setpoint;

  if (strafe_abs_error_ < kStrafeSlopeStart) {
    if (strafe_abs_error_ < kStrafeCloseEnough) {
      setpoint = 0;
    } else {
      setpoint = kStrafeMinSpeed + ((strafe_abs_error_ - kStrafeCloseEnough) /
                                    (kStrafeSlopeStart - kStrafeCloseEnough)) *
                                       (kStrafeMaxSpeed - kStrafeMinSpeed);
    }
  } else {
    setpoint = kStrafeMaxSpeed;
  }
  setpoint = setpoint * (signbit(strafe_error_) ? -1 : 1);

  SPDLOG_DEBUG(logger_, "PlaceGear strafe error = {}, setpoint =  {}",
               strafe_error_, round(setpoint * kSetpointMax));

  Robot::drive->Drive(0, setpoint, 0, kStrafeDeadzone);
}

/**
 * Finished when distance is close enough.
 */
bool PlaceGear::IsFinished() {
  if (!Robot::deadeye->HasTarget()) {
    return false;
  }
  if (strafe_abs_error_ < kStrafeCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    return true;
  }
  return false;
}
