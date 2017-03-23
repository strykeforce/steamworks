#include "deadeye_azimuth.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const float kSetpointMax = 275.0;
const double kMaxSpeed = 100.0 / kSetpointMax;
const double kMinSpeed = 15.0 / kSetpointMax;
const double kMinSpeedExact = 15.0 / kSetpointMax;
const double kDeadZone = 0.1 * kMinSpeedExact;
const int kCloseEnough = 20;
const int kCloseEnoughExact = 8;
const int kSlopeStart = 300;
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
  Robot::drive->Drive(0, 0, 0);
  Robot::drive->SetAzimuthMode();
  error_ = Robot::deadeye->GetAzimuthError();
  offset_ = has_offset_ ? Robot::deadeye->GetSolutionAzimuthOffset() : 0;
  logger_->info("DeadeyeAzimuth initialized with error {}", error_, offset_);
  stable_count_ = 0;
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void DeadeyeAzimuth::Execute() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->info("DeadeyeAzimuth::Execute no target");
    Robot::drive->Drive(0, 0, 0);
    return;
  }
  error_ = Robot::deadeye->GetAzimuthError() + offset_;
  abs_error_ = fabs(error_);
  double speed;

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnoughExact) {
      speed = 0;
      Robot::drive->ClearDriveIaccum();
    } else if (abs_error_ < kCloseEnough) {
      speed = kMinSpeedExact;
    } else {
      speed = kMinSpeed +
              ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                  (kMaxSpeed - kMinSpeed);
    }
  } else {
    speed = kMaxSpeed;
  }
  speed = speed * (signbit(error_) ? 1 : -1);  // match sign to error
  SPDLOG_DEBUG(logger_, "DeadeyeAzimuth error_ = {}, speed =  {}", error_,
               round(speed * kSetpointMax));
  Robot::drive->Drive(0, 0, speed, kDeadZone);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool DeadeyeAzimuth::IsFinished() {
  if (!Robot::deadeye->HasTarget()) {
    return false;
  }
  if (abs_error_ < kCloseEnoughExact) {
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
