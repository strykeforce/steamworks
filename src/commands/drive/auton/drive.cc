#include "drive.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const double kSetpointMax = 400.0;
const double kMaxSpeed = 180.0 / kSetpointMax;  // 600 practical max
const double kMinSpeed = 42.0 / kSetpointMax;
// const double kMaxAccelStep = kMaxSpeed;
const int kCloseEnough = 25;
const int kSlopeStart = 800;
const int kStableCountReq = 3;
const int kZeroCountReq = 3;
}

/**
 * Designated constructor.
 *
 * @param angle the direction to travel, relative to robot forward -180.0 to
 * 180.0
 * @param distance the number of encoder ticks to travel
 * @param timeout the timecount in seconds
 */
Drive::Drive(double angle, double distance, double timeout)
    : frc::Command("Drive", timeout),
      logger_(spdlog::get("command")),
      angle_(angle),
      distance_(distance) {
  Requires(Robot::drive);
}

Drive::Drive(double angle, double distance) : Drive(angle, distance, -1.0) {}

/**
 * Initialize
 */
void Drive::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->ZeroPosition();
  error_ = distance_;
  forward_factor_ = cos(angle_ * M_PI / 180);
  strafe_factor_ = sin(angle_ * M_PI / 180);
  SPDLOG_DEBUG(logger_, "angle = {}, forward = {}, strafe = {}", angle_,
               forward_factor_, strafe_factor_);
  // start_decel_pos_ = distance_ - kSlopeStart;
  stable_count_ = 0;
  zero_count_ = 0;

  SPDLOG_DEBUG(logger_, "distance = {}, position = {}, kSlopeStart = {}",
               distance_, Robot::drive->GetPosition(), kSlopeStart);
}

/**
 * Execute is called periodically during command execution and sends drive
 * rate commands to the swerve drive based on current error calculations.
 */
void Drive::Execute() {
  // wait a few iterations for Robot::drive->ZeroPosition() to "take"
  if (zero_count_ < kZeroCountReq) {
    abs_error_ = distance_;
    zero_count_++;
    return;
  }

  double position = Robot::drive->GetPosition();  // default to talon 13
  error_ = distance_ - fabs(position);
  abs_error_ = abs(error_);
  double speed;

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnough) {
      speed = 0;
    } else {
      speed = kMinSpeed +
              ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                  (kMaxSpeed - kMinSpeed);
    }
  } /*else if (position < kSlopeStart) {
    speed = kMinSpeed + (position / kSlopeStart) * (kMaxSpeed - kMinSpeed);
  } */
  else {
    speed = kMaxSpeed;
  }

  speed = speed * (signbit(error_) ? -1 : 1);

  SPDLOG_DEBUG(logger_, "Drive position = {}, error = {}, speed = {}", position,
               error_, round(speed * kSetpointMax));

  Robot::drive->Drive(forward_factor_ * speed, strafe_factor_ * speed, 0);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired distance is reached.
 */
bool Drive::IsFinished() {
  if (IsTimedOut()) {
    return true;
  }
  if (abs_error_ < kCloseEnough) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    SPDLOG_DEBUG(logger_, "done with auton drive, abs_error_ = {}", abs_error_);
    return true;
  }
  return false;
}

/**
 * End is called after IsFinished(), it stops drive motion and disables the
 * PID controller loop.
 */
void Drive::End() { Robot::drive->SetDrive(0.0); }
