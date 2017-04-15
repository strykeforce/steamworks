#include "get_angle.h"

#ifdef LOG_DEADEYE
#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif
#include <iomanip>
#include "log.h"
#endif

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/shooter_elevation.h"

using namespace steamworks::command::shooter;
using namespace std;

// tuning parameters
namespace {
const int kMaxSpeed = 70;
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
#ifdef LOG_DEADEYE
  InitializeTelemetry();
#endif
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current error calculations.
 */
void GetAngle::Execute() {
  has_target_ = Robot::deadeye->HasTarget();

#ifdef LOG_DEADEYE
  LogTelemetry();
#endif

  if (!has_target_) {
    logger_->info("GetAngle::Execute no target");
    return;
  }

  error_ = Robot::deadeye->GetCenterlineError();
  abs_error_ = fabs(error_);

  if (abs_error_ < kSlopeStart) {
    if (abs_error_ < kCloseEnough) {
      delta_ = 0;
    } else {
      delta_ = kMinSpeed +
               ((abs_error_ - kCloseEnough) / (kSlopeStart - kCloseEnough)) *
                   (kMaxSpeed - kMinSpeed);
    }
  } else {
    delta_ = kMaxSpeed;
  }
  delta_ = delta_ * (signbit(error_) ? -1 : 1);  // match sign to error
  setpoint_ = Robot::shooter_elevation->GetElevationSetpoint() +
              static_cast<int>(delta_);
  SPDLOG_DEBUG(logger_, "GetAngle error = {}, delta =  {}, setpoint = {}",
               error_, round(delta_), setpoint_);
  Robot::shooter_elevation->SetElevation(setpoint_);
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
  int setpoint = Robot::shooter_elevation->GetElevationSetpoint();
  logger_->info(
      "GetAngle ended with setpoint {}, elevation {} and pixel error {}",
      setpoint, elevation, error_);
  if (!Robot::deadeye->CalculateSolution(elevation)) {
    logger_->warn("GetAngle invalid shooting solution");
    DriverStation::ReportWarning(
        "Invalid shooting solution, cancelling shooter sequence");
    frc::CommandGroup* group = GetGroup();
    if (group != nullptr) {
      group->Cancel();
    }
  }
#ifdef LOG_DEADEYE
  EndTelemetry();
#endif
}

#ifdef LOG_DEADEYE
namespace {
const string kTelemetryPath = "/home/lvuser/logs/get_angle_";
}

/**
 * Open log file for telemetry.
 */
void GetAngle::InitializeTelemetry() {
  string path = Log::GetTelemetryFilePath(kTelemetryPath);
  logger_->info("GetAngle logging telemetry to {}", path);
  telemetry_ = make_unique<ofstream>(path, ofstream::trunc);
  *telemetry_ << "timestamp,has_target,error,delta,setpoint\n";
  telemetry_start_ = timer_.GetFPGATimestamp();
}

/**
 * Log a line of telemetry.
 */
void GetAngle::LogTelemetry() {
  *telemetry_ << setprecision(0) << fixed
              << (timer_.GetFPGATimestamp() - telemetry_start_) * 1000 << ","
              << has_target_ * 10 << "," << setprecision(2) << error_ << ","
              << delta_ << "," << setpoint_ << "\n";
}

/**
 * Close telemetry log file.
 */
void GetAngle::EndTelemetry() { telemetry_->close(); }

#endif
