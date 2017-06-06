#include "gyro_azimuth.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const float kSetpointMax = 275.0;
const float kMaxSpeed = 275.0 / kSetpointMax;
const float kMinSpeed = 25.0 / kSetpointMax;
const float kDeadZone = kMinSpeed * 0.1;

const int kCloseEnough = 4;
const int kSlopeStart = 30;
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
  float initial = RobotMap::gyro->GetAngle();
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
  error_ = target_ - RobotMap::gyro->GetAngle();
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
  speed = speed * (signbit(error_) ? -1 : 1);  // match sign to error
  SPDLOG_DEBUG(logger_, "GyroAzimuth error_ = {}, speed =  {}", error_,
               round(speed * kSetpointMax));
  Robot::drive->Drive(0, 0, speed, kDeadZone);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool GyroAzimuth::IsFinished() {
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    SPDLOG_DEBUG(logger_, "GyroAzimuth done with azimuth, abs_error_ = {}",
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
