#include "place_gear.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"

using namespace steamworks::command::gear;
using namespace std;

namespace {
const double kSetpointMax = 275.0;
const double kTicksPerInch = 50.72;

const double kDriveSpeed = -50.0 / kSetpointMax;
const double kCruiseRatio = 0.25;
const double kStoppedSpeed = 10.0;

const double kStrafeOffset = 50;
const double kStrafeEndDistance = 12 * kTicksPerInch;

const double kStrafeMinSpeed = 0.0 / kSetpointMax;
const double kStrafeMaxSpeed = 40.0 / kSetpointMax;
const double kStrafeSlopeStart = 80;
const double kStrafeCloseEnough = 3;

const double kAzimuthMinSpeed = 15.0 / kSetpointMax;
const double kAzimuthMaxSpeed = 50.0 / kSetpointMax;
const double kAzimuthSlopeStart = 100;
const double kAzimuthCloseEnough = 3;

const double kDeadzone = 1.0 / kSetpointMax;

const int kStableCountReq = 3;
const int kZeroCountReq = 3;

const double kLeftDistancePrac = 12 * kTicksPerInch;
const double kLeftDistanceComp = 12 * kTicksPerInch;
const double kCenterDistancePrac = 100 * kTicksPerInch;
const double kCenterDistanceComp = 20 * kTicksPerInch;
const double kRightDistancePrac = 12 * kTicksPerInch;
const double kRightDistanceComp = 12 * kTicksPerInch;
}

PlaceGear::PlaceGear(Lift position)
    : frc::Command("PlaceGear"), logger_(spdlog::get("command")) {
  Requires(Robot::drive);
  auto practice = RobotMap::IsPracticeRobot();
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

bool PlaceGear::CalculateDistance() {
  if (!Robot::deadeye->HasTarget()) {
    logger_->warn("PlaceGear::CalculateDistance has no target");
    return false;
  }
  auto height = Robot::deadeye->GetTargetHeight();
  auto range = 4183.3 * std::pow(height, -1.039);
  logger_->info("PlaceGear initial target height = {}, range = {}", height,
                range);
  // return range;
  distance_ = range * kTicksPerInch;
  return true;
}

/**
 * Initialize
 */
void PlaceGear::Initialize() {
  Robot::drive->SetAzimuthMode();
  Robot::drive->ZeroPosition();
  Robot::drive->SetGyroDisabled(true);

  CalculateDistance();

  strafe_error_ = Robot::deadeye->GetStrafeError() - kStrafeOffset;
  azimuth_error_ = RobotMap::gyro->GetAngle() - azimuth_angle_;

  stable_count_ = 0;
  zero_count_ = 0;
  is_delay_done_ = false;
  is_cruising_ = false;
  is_initialized = false;

  logger_->info(
      "PlaceGear initialized with distance = {}, strafe error = {}, azimuth "
      "error = {}",
      distance_, strafe_error_, azimuth_error_);
}

/**
 * Calculate drive forward setpoint based on camera azimuth error.
 */
double PlaceGear::CalculateDriveSetpoint() { return kDriveSpeed; }

/**
 * Calculate strafe setpoint based on camera strafe error.
 */
double PlaceGear::CalculateStrafeSetpoint() {
  if (distance_ - std::fabs(Robot::drive->GetPosition()) < kStrafeEndDistance) {
    return 0;
  }

  if (!Robot::deadeye->HasTarget()) {
    logger_->warn(
        "PlaceGear has no target, strafe error = {}, setpoint override = 0",
        strafe_error_);
    return 0;
  }
  strafe_error_ = Robot::deadeye->GetStrafeError() - kStrafeOffset;
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
  setpoint = setpoint * (signbit(strafe_error_) ? -1 : 1);
  SPDLOG_DEBUG(logger_, "PlaceGear strafe error = {}, setpoint =  {}",
               strafe_error_, round(setpoint * kSetpointMax));
  return setpoint;
}

/**
 * Calculate azimuth setpoint based on gyro azimuth error.
 */
double PlaceGear::CalculateAzimuthSetpoint() {
  return 0;
  /*
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
  */
}

/**
 * Wait for Robot::drive->ZeroPosition() to "take"
 */
inline bool PlaceGear::DelayForPositionZero() {
  if (is_delay_done_) return false;
  // wait a few iterations for Robot::drive->ZeroPosition() to "take"
  if (zero_count_ < kZeroCountReq) {
    drive_abs_error_ = distance_;
    zero_count_++;
    return true;
  }
  is_delay_done_ = true;
  return false;
}

/**
 * Execute the main loop
 */
void PlaceGear::Execute() {
  if (!is_initialized) {
    SPDLOG_DEBUG(logger_, "PlaceGear in initialization loop");
    if (DelayForPositionZero()) {
      return;
    }

    if (!CalculateDistance()) {
      // FIXME: give up checking and use default if can't find
      return;
    }
    is_initialized = true;
  }

  auto drive_setpoint = CalculateDriveSetpoint();
  auto strafe_setpoint = CalculateStrafeSetpoint();
  auto azimuth_setpoint = CalculateAzimuthSetpoint();

  Robot::drive->Drive(drive_setpoint, strafe_setpoint, azimuth_setpoint,
                      kDeadzone);
  logger_->debug("PlaceGear drive = {}, strafe = {}, azimuth = {}",
                 std::round(drive_setpoint * kSetpointMax),
                 std::round(strafe_setpoint * kSetpointMax),
                 std::round(azimuth_setpoint * kSetpointMax));
}

/**
 * Finished when distance is close enough.
 */
bool PlaceGear::IsFinished() {
  if (!is_initialized) return false;
  double speed = fabs(Robot::drive->GetDriveSpeed());
  // SPDLOG_DEBUG(logger_, "IsFinished avg speed = {}", speed);
  logger_->debug("IsFinished avg speed = {}", speed);

  if (!is_cruising_) {
    // SPDLOG_DEBUG(logger_, "IsFinished not cruising");
    logger_->debug("IsFinished not cruising");
    is_cruising_ = speed > kCruiseRatio * -kDriveSpeed * kSetpointMax;
    return false;
  }

  if (speed < kStoppedSpeed) {
    stable_count_++;
  } else {
    stable_count_ = 0;
  }
  SPDLOG_DEBUG(logger_, "IsFinished stable count = {}", stable_count_);
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
  logger_->info("PlaceGear ended with strafe error = {}, azimuth error = {}",
                strafe_error_, azimuth_error_);
}
