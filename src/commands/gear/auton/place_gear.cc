#include "place_gear.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"

using namespace steamworks::command::gear;
using namespace std;

namespace {
const double kSetpointMax = 275.0;

const double kDriveMinSpeed = 15.0 / kSetpointMax;
const double kDriveMaxSpeed = 50.0 / kSetpointMax;
const double kDriveSlopeStart = 100;
const double kDriveCloseEnough = 5;

const double kStrafeMinSpeed = 15.0 / kSetpointMax;
const double kStrafeMaxSpeed = 50.0 / kSetpointMax;
const double kStrafeSlopeStart = 60;
const double kStrafeCloseEnough = 3;

const double kAzimuthMinSpeed = 15.0 / kSetpointMax;
const double kAzimuthMaxSpeed = 50.0 / kSetpointMax;
const double kAzimuthSlopeStart = 100;
const double kAzimuthCloseEnough = 3;

const double kDeadzone = 1.0 / kSetpointMax;

const int kStableCountReq = 3;
const int kZeroCountReq = 3;

const double kTicksPerInch = 50.72;
const double kLeftDistancePrac = 12 * kTicksPerInch;
const double kLeftDistanceComp = 12 * kTicksPerInch;
const double kCenterDistancePrac = 20 * kTicksPerInch;
const double kCenterDistanceComp = 20 * kTicksPerInch;
const double kRightDistancePrac = 12 * kTicksPerInch;
const double kRightDistanceComp = 12 * kTicksPerInch;
}

PlaceGear::PlaceGear(Lift position)
    : frc::Command("PlaceGear"), logger_(spdlog::get("command")) {
  Requires(Robot::drive);
  auto practice = RobotMap::IsPracticeRobot();
  strafe_offset_ = 0;
  switch (position) {
    case Lift::left:
      distance_ = practice ? kLeftDistancePrac : kLeftDistanceComp;
      azimuth_angle_ = 60.0;
      break;
    case Lift::center:
      distance_ = practice ? kCenterDistancePrac : kCenterDistanceComp;
      azimuth_angle_ = 0.0;
      break;
    case Lift::right:
      distance_ = practice ? kRightDistancePrac : kRightDistanceComp;
      azimuth_angle_ = -60.0;
      break;
  }
}

/**
 * Initialize
 */
void PlaceGear::Initialize() {
  Robot::drive->SetAzimuthMode();
  Robot::drive->ZeroPosition();
  Robot::drive->SetGyroDisabled(true);

  drive_error_ = distance_;
  strafe_error_ = Robot::deadeye->GetStrafeError() - strafe_offset_;
  azimuth_error_ = RobotMap::gyro->GetAngle() - azimuth_angle_;

  stable_count_ = 0;
  zero_count_ = 0;

  logger_->info(
      "PlaceGear initialized with distance = {}, strafe error = {}, azimuth "
      "error = {}",
      distance_, strafe_error_, azimuth_error_);
}

/**
 * Calculate drive forward setpoint based on camera azimuth error.
 */
double PlaceGear::CalculateDriveSetpoint() {
  double position = Robot::drive->GetPosition();  // default to talon 13
  drive_error_ = distance_ - fabs(position);
  drive_abs_error_ = fabs(drive_error_);
  double setpoint;

  if (drive_abs_error_ < kDriveSlopeStart) {
    if (drive_abs_error_ < kDriveCloseEnough) {
      setpoint = 0;
    } else {
      setpoint = kDriveMinSpeed + ((drive_abs_error_ - kDriveCloseEnough) /
                                   (kDriveSlopeStart - kDriveCloseEnough)) *
                                      (kDriveMaxSpeed - kDriveMinSpeed);
    }
  } else {
    setpoint = kDriveMaxSpeed;
  }
  setpoint = setpoint * (signbit(drive_error_) ? 1 : -1);

  SPDLOG_DEBUG(logger_, "PlaceGear drive error = {}, drive setpoint = {}",
               drive_error_, round(setpoint * kSetpointMax));

  return setpoint;
}

/**
 * Calculate strafe setpoint based on camera strafe error.
 */
double PlaceGear::CalculateStrafeSetpoint() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->warn(
        "PlaceGear has no target, strafe error = {}, setpoint override = 0",
        strafe_error_);
    return 0;
  }
  strafe_error_ = Robot::deadeye->GetStrafeError() - strafe_offset_;
  strafe_abs_error_ = fabs(strafe_error_);
  double setpoint;

  if (strafe_abs_error_ < kStrafeSlopeStart) {
    if (strafe_abs_error_ < kStrafeCloseEnough) {
      setpoint = 0;
    } else {
      setpoint = kStrafeMinSpeed + ((strafe_abs_error_ - kStrafeCloseEnough) /
                                    (kStrafeSlopeStart - kStrafeCloseEnough)) *
                                       (kStrafeMaxSpeed - kStrafeMinSpeed);
    }
  } else {
    setpoint = kStrafeMaxSpeed;
  }
  setpoint = setpoint * (signbit(strafe_error_) ? 1 : -1);
  SPDLOG_DEBUG(logger_, "PlaceGear strafe error = {}, setpoint =  {}",
               strafe_error_, round(setpoint * kSetpointMax));
  return setpoint;
}

/**
 * Calculate azimuth setpoint based on gyro azimuth error.
 */
double PlaceGear::CalculateAzimuthSetpoint() {
  azimuth_error_ = Robot::deadeye->GetStrafeError();
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
 * Execute the main loop
 */
void PlaceGear::Execute() {
  // wait a few iterations for Robot::drive->ZeroPosition() to "take"
  if (zero_count_ < kZeroCountReq) {
    drive_abs_error_ = distance_;
    zero_count_++;
    return;
  }

  auto distance_setpoint = CalculateDriveSetpoint();
  auto strafe_setpoint = CalculateStrafeSetpoint();
  auto azimuth_setpoint = CalculateAzimuthSetpoint();

  Robot::drive->Drive(distance_setpoint, strafe_setpoint, azimuth_setpoint,
                      kDeadzone);
}

/**
 * Finished when distance is close enough.
 */
bool PlaceGear::IsFinished() {
  if (Robot::drive->GetPosition() >= distance_) {
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
 * Finis
 */
void PlaceGear::End() {
  Robot::drive->SetGyroDisabled(false);
  logger_->info(
      "PlaceGear ended with drive error = {}, strafe error = {}, azimuth "
      "error = {}",
      drive_error_, strafe_error_, azimuth_error_);
}
