#include "azimuth.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command;
using namespace std;

// tuning parameters
namespace {
const float kMaxSpeed = 75;
const float kMinSpeed = 10;
const int kCloseEnough = 2;
const int kSlopeStart = 20;
const int kStableCountReq = 3;
}

/**
 * DriveAzimuth is a command to spin the robot to a given field-relative
 * azimuth.
 */
DriveAzimuth::DriveAzimuth(float target)
    : frc::Command("DriveAzimuth"),
      logger_(spdlog::get("command")),
      target_(target) {
  Requires(Robot::drive);
}

/**
 * Initialize starts the PID controller loop.
 */
void DriveAzimuth::Initialize() {
  Robot::drive->SetAzimuthMode();
  float initial = RobotMap::gyro->GetYaw();
  error_ = target_ - initial;
  logger_->debug("target = {}, initial = {}, error = {}", target_, initial,
                 error_);
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void DriveAzimuth::Execute() {
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
  logger_->debug("abs_error_ = {}, error_ = {}, speed =  {}", abs_error_,
                 error_, speed);
  Robot::drive->DriveAzimuthAutonomous(speed);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired azimuth is reached.
 */
bool DriveAzimuth::IsFinished() {
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
    logger_->trace("incrementing stable_count_ to {}", stable_count_);
  } else {
    stable_count_ = 0;
    logger_->trace("resetting stable_count_ to 0");
  }
  if (stable_count_ == kStableCountReq) {
    logger_->debug("done with DriveAzimuth auton azimuth");
    return true;
  }
  return false;
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void DriveAzimuth::End() { Robot::drive->SetDrive(0.0); }

//
// PositionAzimuth
//
PositionAzimuth::PositionAzimuth() : frc::Command("PositionAzimuth") {
  Requires(Robot::drive);
}

/**
 * Initial send the position to the azimuth motors.
 */
void PositionAzimuth::Initialize() { Robot::drive->PositionAzimuthForAuton(); }

/**
 * Azimuth wheels are close enough
 */
bool PositionAzimuth::IsFinished() {
  return Robot::drive->IsPositionAzimuthForAutonDone();
}
