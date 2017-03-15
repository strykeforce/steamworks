#include "drive.h"

#include "robot.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const double kMaxSpeed = 200;  // 600 practical max
const double kMinSpeed = 25;

const double kMaxSpeedTime = 400;
const double kApproxPeriod = 20;

const int kCloseEnough = 5;
const int kStableCountReq = 3;

const int kAccelIterCount =
    static_cast<unsigned>(ceil(kMaxSpeedTime / kApproxPeriod));

const int kSlopeDistance = kMaxSpeed * (kMaxSpeedTime / 100) * 0.5;
}

/**
 * Designated constructor.
 */
Drive::Drive(int distance, int azimuth, double timeout)
    : frc::Command("Drive"),
      logger_(spdlog::get("command")),
      distance_(distance),
      azimuth_(azimuth),
      timeout_(timeout) {
  Requires(Robot::drive);
}

/**
 * Initialize with default azimuth of zero.
 */
Drive::Drive(int distance) : Drive(distance, 0, -1) {}

/**
 * Initialize
 */
void Drive::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->ZeroPosition();
  error_ = distance_;
  SPDLOG_DEBUG(logger_, "target = {}", distance_);
  start_decel_pos_ = distance_ - kSlopeDistance;
  stable_count_ = 0;

  SPDLOG_DEBUG(
      logger_,
      "distance = {}, position = {}, kSlopeDistance = {}, start_decel_pos_ = "
      "{}",
      distance_, Robot::drive->GetPosition(), kSlopeDistance, start_decel_pos_);
  if (timeout_ != -1) {
    SetTimeout(timeout_);
  }
}

/**
 * Execute is called periodically during command execution and sends drive
 * rate commands to the swerve drive based on current error calculations.
 */
void Drive::Execute() {
  double position = Robot::drive->GetPosition();
  if (position < 0) {
    position = 0;
  }
  error_ = distance_ - position;
  abs_error_ = abs(error_);
  double speed;

  if (abs_error_ < kSlopeDistance) {
    if (abs_error_ < kCloseEnough) {
      speed = 0;
    } else {
      speed = kMinSpeed +
              ((abs_error_ - kCloseEnough) / (kSlopeDistance - kCloseEnough)) *
                  (kMaxSpeed - kMinSpeed);
    }
  } else if (position < kSlopeDistance) {
    speed = kMinSpeed + (position / kSlopeDistance) * (kMaxSpeed - kMinSpeed);
  } else {
    speed = kMaxSpeed;
  }

  speed = speed * (signbit(error_) ? -1 : 1);

  SPDLOG_DEBUG(logger_, "Drive position = {}, error = {}, speed = {}", position,
               error_, speed);
  Robot::drive->CrabDriveAutonomous(speed, azimuth_);
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