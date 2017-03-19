#include "drive.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

// tuning parameters
namespace {
const double kSetpointMax = 400.0;
const double kDeadZonePct = 0.9;
const int kStableCountReq = 3;
const int kZeroCountReq = 3;
}

/**
 * Designated constructor.
 *
 * @param config the object containing configuration parameters
 */
Drive::Drive(const DriveConfig& config)
    : frc::Command("Drive", config.timeout),
      logger_(spdlog::get("command")),
      angle_(config.angle),
      distance_(config.distance),
      min_speed_(config.min_speed / kSetpointMax),
      max_speed_(config.max_speed / kSetpointMax),
      close_enough_(config.close_enough) {
  Requires(Robot::drive);
  accel_dist_ = pow(config.max_speed, 2) / (2 * config.acceleration);
  deaccel_dist_ = pow(config.max_speed, 2) / (2 * config.deacceleration);
  accel_done_pos_ = distance_ - accel_dist_;
  dead_zone_ = kDeadZonePct * min_speed_;
}

/**
 * Initialize
 */
void Drive::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->ZeroPosition();
  error_ = distance_;
  forward_factor_ = cos(angle_ * M_PI / 180);
  strafe_factor_ = sin(angle_ * M_PI / 180);
  SPDLOG_DEBUG(logger_, "Drive angle = {}, forward = {}, strafe = {}", angle_,
               forward_factor_, strafe_factor_);
  // start_decel_pos_ = distance_ - kSlopeStart;
  stable_count_ = 0;
  zero_count_ = 0;
  logger_->info(
      "Drive initialized with angle = {}, distance = {}, min speed = {}, max "
      "speed = {}, close enough = {}, accel dist = {}, deaccel dist = {}, "
      "accel done pos = {}",
      angle_, distance_, min_speed_ * kSetpointMax, max_speed_ * kSetpointMax,
      close_enough_, accel_dist_, deaccel_dist_, accel_done_pos_);
  // data_.reset std::ofstream file{kCalibrationPath, std::ofstream::trunc};
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
  double speed = 0;

  if (abs_error_ > accel_done_pos_) {  // not done accelerating
    speed = min_speed_ + ((max_speed_ - min_speed_) / accel_dist_) *
                             (distance_ - abs_error_);
  } else if (abs_error_ < deaccel_dist_) {
    if (abs_error_ < close_enough_) {
      speed = 0;
    } else {
      speed = min_speed_ +
              ((abs_error_ - close_enough_) / (deaccel_dist_ - close_enough_)) *
                  (max_speed_ - min_speed_);
    }
  } else {
    speed = max_speed_;
  }

  speed = speed * (signbit(error_) ? -1 : 1);

  SPDLOG_DEBUG(logger_, "Drive position = {}, error = {}, speed = {}", position,
               error_, round(speed * kSetpointMax));

  Robot::drive->Drive(forward_factor_ * speed, strafe_factor_ * speed, 0,
                      dead_zone_);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired distance is reached.
 */
bool Drive::IsFinished() {
  if (IsTimedOut()) {
    return true;
  }
  if (abs_error_ < close_enough_) {
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
