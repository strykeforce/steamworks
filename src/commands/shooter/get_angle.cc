#include "get_angle.h"

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/shooter_elevation.h"

using namespace steamworks::command::shooter;
using namespace std;

// tuning parameters
namespace {
const int kMaxSpeed = 45;
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
  Requires(Robot::shooter_elevation);
}

/**
 * Initialize starts the controller loop.
 */
void GetAngle::Initialize() {
  error_ = Robot::deadeye->GetCenterlineError();
  logger_->info("GetAngle initialized with error {}", error_);
  stable_count_ = 0;
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void GetAngle::Execute() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->info("GetAngle::Execute no target");
    return;
  }

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
  int pos = Robot::shooter_elevation->GetElevationSetpoint();
  SPDLOG_DEBUG(logger_, "GetAngle error_ = {}, ticks =  {}, setpoint = {}",
               error_, ticks, pos + static_cast<int>(ticks));
  Robot::shooter_elevation->SetElevation(pos + static_cast<int>(ticks));
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool GetAngle::IsFinished() {
  if (!Robot::deadeye->HasTarget()) {
    return false;
  }

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
 * Called once if this command is interrupted.
 */
void GetAngle::Interrupted() {
  logger_->info("GetAngle interrupted");
  End();
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void GetAngle::End() {
  int elevation = Robot::shooter_elevation->GetElevation();
  logger_->info("GetAngle ended with elevation {} and pixel error {}",
                elevation, error_);
  if (!Robot::deadeye->CalculateSolution(elevation)) {
    logger_->warn("GetAngle invalid shooting solution");
    DriverStation::ReportWarning(
        "Invalid shooting solution, cancelling shooter sequence");
    frc::CommandGroup* group = GetGroup();
    if (group != nullptr) {
      group->Cancel();
    }
  }
}
