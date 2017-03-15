#include "gyro_azimuth.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const float kMaxSpeed = 75.0 / 75.0;
const float kMinSpeed = 10.0 / 75.0;
const int kCloseEnough = 4;
const int kSlopeStart = 100;
const int kStableCountReq = 3;
}

/**
 * GyroAzimuth is a command to spin the robot to a given field-relative
 * azimuth.
 */
GyroAzimuth::GyroAzimuth(float target)
    : frc::Command("GyroAzimuth"),
      logger_(spdlog::get("command")),
      target_(target) {
  Requires(Robot::drive);
}

/**
 * Initialize starts the PID controller loop.
 */
void GyroAzimuth::Initialize() {
  Robot::drive->SetAzimuthMode();
  float initial = RobotMap::gyro->GetYaw();
  error_ = target_ - initial;
  SPDLOG_DEBUG(logger_, "target = {}, initial = {}, error = {}", target_,
               initial, error_);
  stable_count_ = 0;
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void GyroAzimuth::Execute() {
  error_ = target_ - RobotMap::gyro->GetYaw();
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
  SPDLOG_DEBUG(logger_, "abs_error_ = {}, error_ = {}, speed =  {}", abs_error_,
               error_, speed);
  // Robot::drive->DriveAzimuthAutonomous(speed);
  Robot::drive->DriveAutonomous(0, 0, speed);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool GyroAzimuth::IsFinished() {
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
    SPDLOG_TRACE(logger_, "incrementing stable_count_ to {}", stable_count_);
  } else {
    stable_count_ = 0;
    SPDLOG_TRACE(logger_, "resetting stable_count_ to 0");
  }
  if (stable_count_ == kStableCountReq) {
    SPDLOG_DEBUG(logger_, "done with auton azimuth, abs_error_ = {}",
                 abs_error_);
    return true;
  }
  return false;
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void GyroAzimuth::End() { Robot::drive->SetDrive(0.0); }