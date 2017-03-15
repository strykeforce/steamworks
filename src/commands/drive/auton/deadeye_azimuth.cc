#include "deadeye_azimuth.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const double kMaxSpeed = 50.0 / 75.0;
const double kMinSpeed = 10.0 / 75.0;
const int kCloseEnough = 4;
const int kSlopeStart = 160;
const int kStableCountReq = 3;
}

/**
 * DeadeyeAzimuth is a command to spin the robot to eliminate camera azimuth
 * error.
 */
DeadeyeAzimuth::DeadeyeAzimuth(bool offset)
    : frc::Command("DeadeyeAzimuth"),
      logger_(spdlog::get("command")),
      has_offset_(offset) {
  Requires(Robot::drive);
}

/**
 * Initialize starts the controller loop.
 */
void DeadeyeAzimuth::Initialize() {
  Robot::drive->SetAzimuthMode();
  error_ = Robot::deadeye->GetAzimuthError();
  offset_ = has_offset_ ? Robot::deadeye->GetSolutionAzimuthOffset() : 0;
  logger_->info("DeadeyeAzimuth initialized with error {} and offset {}",
                error_, offset_);
  stable_count_ = 0;
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void DeadeyeAzimuth::Execute() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->info("DeadeyeAzimuth::Execute no target");
    return;
  }
  error_ = Robot::deadeye->GetAzimuthError() + offset_;
  abs_error_ = fabs(error_);
  double speed;

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnough) {
      speed = 0;
    } else {
      speed = kMinSpeed +
              ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                  (kMaxSpeed - kMinSpeed);
    }
  } else {
    speed = kMaxSpeed;
  }
  speed = speed * (signbit(error_) ? 1 : -1);  // match sign to error
  SPDLOG_DEBUG(logger_, "DeadeyeAzimuth error_ = {}, offset_ = {}, speed =  {}",
               error_, offset_, speed);
  Robot::drive->DriveAutonomous(0, 0, speed);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool DeadeyeAzimuth::IsFinished() {
  if (!Robot::deadeye->HasTarget()) {
    return false;
  }
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    return true;
  }
  return false;
}

/**
 * Called if this command is interrupted.
 */
void DeadeyeAzimuth::Interrupted() {
  logger_->info("DeadeyeAzimuth interrupted");
  End();
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void DeadeyeAzimuth::End() {
  logger_->info("DeadeyeAzimuth ended with error {} and offset {}", error_,
                offset_);
  Robot::drive->SetDrive(0.0);
}
