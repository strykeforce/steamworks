#include "deadeye_azimuth.h"

#ifdef LOG_DEADEYE
#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif
#include <iomanip>
#endif

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
#ifdef LOG_DEADEYE
  InitializeTelemetry();
#endif
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void DeadeyeAzimuth::Execute() {
  has_target_ = Robot::deadeye->HasTarget();

#ifdef LOG_DEADEYE
  LogTelemetry();
#endif

  if (!has_target_) {
    logger_->info("DeadeyeAzimuth::Execute no target");
    Robot::drive->Drive(0, 0, 0);
    return;
  }
  error_ = Robot::deadeye->GetAzimuthError() + offset_;
  abs_error_ = fabs(error_);

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnoughExact) {
      setpoint_ = 0;
      Robot::drive->ClearDriveIaccum();
    } else if (abs_error_ < kCloseEnough) {
      setpoint_ = kMinSpeedExact;
    } else {
      setpoint_ = kMinSpeed +
                  ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                      (kMaxSpeed - kMinSpeed);
    }
  } else {
    setpoint_ = kMaxSpeed;
  }
  setpoint_ = setpoint_ * (signbit(error_) ? 1 : -1);  // match sign to error
  SPDLOG_DEBUG(logger_, "DeadeyeAzimuth error_ = {}, setpoint_ =  {}", error_,
               round(setpoint_ * kSetpointMax));
  Robot::drive->Drive(0, 0, setpoint_, kDeadZone);
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
#ifdef LOG_DEADEYE
  EndTelemetry();
#endif
}

#ifdef LOG_DEADEYE
namespace {
const string kTelemetryPath = "/home/lvuser/logs/deadeye_azimuth.csv";
}

/**
 * Open log file for telemetry.
 */
void DeadeyeAzimuth::InitializeTelemetry() {
  telemetry_ = make_unique<ofstream>(kTelemetryPath, ofstream::trunc);
  *telemetry_ << "timestamp,has_target,error,setpoint,angle\n";
  telemetry_start_ = timer_.GetFPGATimestamp();
}

/**
 * Log a line of telemetry.
 */
void DeadeyeAzimuth::LogTelemetry() {
  *telemetry_ << setprecision(0) << fixed
              << (timer_.GetFPGATimestamp() - telemetry_start_) * 1000 << ","
              << has_target_ << "," << setprecision(2) << error_ << ","
              << setpoint_ * kSetpointMax << "," << RobotMap::gyro->GetAngle()
              << "\n";
}

/**
 * Close telemetry log file.
 */
void DeadeyeAzimuth::EndTelemetry() { telemetry_->close(); }

#endif
