#include "get_angle.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command::shooter;
using namespace std;

// tuning parameters
namespace {
const int kMaxSpeed = 50;
const int kMinSpeed = 1;
const int kCloseEnough = 3;
const int kSlopeStart = 200;
const int kStableCountReq = 3;
}

/**
 * GetAngle is a command to spin the robot to eliminate camera azimuth
 * error.
 */
GetAngle::GetAngle()
    : frc::Command("GetAngle"), logger_(spdlog::get("command")) {
  Requires(Robot::deadeye);
  Requires(Robot::shooter);
}

/**
 * Initialize starts the controller loop.
 */
void GetAngle::Initialize() {
  error_ = Robot::deadeye->GetCenterlineError();
  SPDLOG_DEBUG(logger_, "GetAngle centerline error = {}", error_);
  stable_count_ = 0;
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void GetAngle::Execute() {
  error_ = Robot::deadeye->GetCenterlineError();
  abs_error_ = fabs(error_);
  double ticks;

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnough) {
      ticks = 0;
    } else {
      ticks = kMinSpeed +
              ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                  (kMaxSpeed - kMinSpeed);
    }
  } else {
    ticks = kMaxSpeed;
  }
  ticks = ticks * (signbit(error_) ? -1 : 1);  // match sign to error
  int pos = Robot::shooter->GetElevationSetpoint();
  SPDLOG_DEBUG(logger_,
               "abs_error_ = {}, error_ = {}, ticks =  {}, setpoint = {}",
               abs_error_, error_, ticks, pos + static_cast<int>(ticks));
  Robot::shooter->SetElevation(pos + static_cast<int>(ticks));
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool GetAngle::IsFinished() {
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    SPDLOG_DEBUG(logger_, "done with centerline, abs_error_ = {}", abs_error_);
    return true;
  }
  return false;
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void GetAngle::End() {
  int elevation = Robot::shooter->GetElevation();
  Robot::shooter->SetSolutionInputs(elevation /*, error_*/);
  SPDLOG_DEBUG(logger_,
               "setting shooter solution inputs: elevation = {}, pixels = {}",
               elevation, error_);
}
