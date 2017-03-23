#include "place_gear.h"

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"

using namespace steamworks::command::gear;
using namespace std;

namespace {
const double kSetpointMax = 275.0;

const double kDistanceTargetHeight = 266.0;
const double kDistanceMinSpeed = 15.0 / kSetpointMax;
const double kDistanceMaxSpeed = 50.0 / kSetpointMax;
const double kDistanceSlopeStart = 100;
const double kDistanceCloseEnough = 5;

const double kAzimuthMinSpeed = 15.0 / kSetpointMax;
const double kAzimuthMaxSpeed = 50.0 / kSetpointMax;
const double kAzimuthSlopeStart = 100;
const double kAzimuthCloseEnough = 3;

const int kStableCountReq = 3;
}

PlaceGear::PlaceGear()
    : frc::Command("PlaceGear"), logger_(spdlog::get("command")) {
  Requires(Robot::drive);
}

/**
 * Initialize
 */
void PlaceGear::Initialize() {
  Robot::drive->SetAzimuthMode();
  azimuth_error_ = Robot::deadeye->GetAzimuthError();
  height_error_ = height_ - Robot::deadeye->GetTargetHeight();
  logger_->info("PlaceGear initialized with azimuth {} and target height {}",
                azimuth_error_, height_error_);
  stable_count_ = 0;
}

/**
 * Calculate azimuth setpoint based on camera azimuth error.
 */
double PlaceGear::CalculateAzimuthSetpoint() {
  azimuth_error_ = Robot::deadeye->GetAzimuthError();
  azimuth_abs_error_ = fabs(azimuth_error_);
  double setpoint;

  if (azimuth_abs_error_ < kAzimuthSlopeStart) {
    if (azimuth_abs_error_ < kAzimuthCloseEnough) {
      setpoint = 0;
    } else {
      setpoint =
          kAzimuthMinSpeed + ((azimuth_abs_error_ - kAzimuthCloseEnough) /
                              (kAzimuthSlopeStart - kAzimuthCloseEnough)) *
                                 (kAzimuthMaxSpeed - kAzimuthMinSpeed);
    }
  } else {
    setpoint = kAzimuthMaxSpeed;
  }
  setpoint = setpoint * (signbit(azimuth_error_) ? 1 : -1);
  SPDLOG_DEBUG(logger_, "PlaceGear azimuth error = {}, setpoint =  {}",
               azimuth_error_, round(setpoint * kSetpointMax));
  return setpoint;
}

/**
 * Calculate drive forward setpoint based on camera azimuth error.
 */
double PlaceGear::CalculateDriveSetpoint() {
  height_error_ = kDistanceTargetHeight - Robot::deadeye->GetTargetHeight();
  height_abs_error_ = fabs(height_error_);
  double setpoint;

  if (height_abs_error_ < kDistanceSlopeStart) {
    if (height_abs_error_ < kDistanceCloseEnough) {
      setpoint = 0;
    } else {
      setpoint =
          kDistanceMinSpeed + ((height_abs_error_ - kDistanceCloseEnough) /
                               (kDistanceSlopeStart - kDistanceCloseEnough)) *
                                  (kDistanceMaxSpeed - kDistanceMinSpeed);
    }
  } else {
    setpoint = kDistanceMaxSpeed;
  }
  setpoint = setpoint * (signbit(height_error_) ? 1 : -1);

  SPDLOG_DEBUG(logger_, "PlaceGear target height error = {}, setpoint = {}",
               height_error_, round(setpoint * kSetpointMax));

  return setpoint;
}

/**
 * Execute the main loop
 */
void PlaceGear::Execute() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->warn("PlaceGear::Execute no target");
    return;
  }
  // azimuth_error_ = Robot::deadeye->GetAzimuthError();
  // height_error_ = height_ - Robot::deadeye->GetTargetHeight();

  auto azimuth_setpoint = CalculateAzimuthSetpoint();
  auto distance_setpoint = CalculateDriveSetpoint();

  Robot::drive->Drive(distance_setpoint, 0, azimuth_setpoint);
}

/**
 * Finished when distance is close enough.
 */
bool PlaceGear::IsFinished() {
  // return false;
  if (!Robot::deadeye->HasTarget()) {
    return false;
  }
  if (Robot::deadeye->GetTargetHeight() > kDistanceTargetHeight) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  if (stable_count_ == kStableCountReq) {
    return true;
  }
  return false;
}
