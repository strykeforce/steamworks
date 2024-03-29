#include "drive.h"

#include <sidewinder/talon/settings.h>

#include "commands/drive/tele_op.h"
#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

/**
 * SwerveDrive is the drive subsystem and sets DriveTeleOp as its default
 * command.
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config)
    : swerve::SwerveDrive(config, RobotMap::swerve_talons, RobotMap::gyro),
      logger_(spdlog::get("subsystem")) {
  // swerve_drive_.SetGyroDisabled(true);
  LoadConfigSettings(config);
}

/**
 * Configure the drive talons with the "drive" settings in SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetTeleOpMode() {
  logger_->info("SetTeleOpMode drive_settings_->GetSetpointMax = {}",
                drive_settings_->GetSetpointMax());
  // assert(drive_settings_);
  SetDriveMode(drive_settings_, drive_settings_->GetSetpointMax());
}

/**
 * Configure the drive talons with the "drive_auton" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAutonMode() {
  assert(drive_auton_settings_);
  SetDriveMode(drive_auton_settings_, drive_auton_settings_->GetSetpointMax());
}

/**
 * Configure the drive talons with the "drive_auton_slow_azimuth" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAzimuthMode() {
  assert(drive_azimuth_settings_);
  SetDriveMode(drive_azimuth_settings_,
               drive_azimuth_settings_->GetSetpointMax());
}

/**
* Set azimuth motors in voltage control mode for testing.
*/
void SwerveDrive::SetAzimuthTestModeEnabled(bool enabled) {
  if (enabled) {
    map_->lf_azimuth->SetControlMode(frc::CANSpeedController::kVoltage);
    map_->rf_azimuth->SetControlMode(frc::CANSpeedController::kVoltage);
    map_->lr_azimuth->SetControlMode(frc::CANSpeedController::kVoltage);
    map_->rr_azimuth->SetControlMode(frc::CANSpeedController::kVoltage);
    return;
  }
  map_->lf_azimuth->SetControlMode(frc::CANSpeedController::kPosition);
  map_->rf_azimuth->SetControlMode(frc::CANSpeedController::kPosition);
  map_->lr_azimuth->SetControlMode(frc::CANSpeedController::kPosition);
  map_->rr_azimuth->SetControlMode(frc::CANSpeedController::kPosition);
}

/**
 * Configure the drive talons with the "drive_auton_motion_magic" settings in
 * SIDEWINDER SWERVE.
 */
void SwerveDrive::SetMotionMagicMode() {
  assert(drive_motion_magic_settings_);
  SetDriveMode(drive_motion_magic_settings_, 0);
}

namespace {
// azimuth positions used when starting at 2048 azimuth
const double kWheel1AzimuthAutonPos = (40.4 / 180) * 2048 + 2048;
const double kWheel2AzimuthAutonPos = (139.6 / 180) * 2048;
const double kWheel3AzimuthAutonPos = (40.4 / 180) * 2048 + 2048;
const double kWheel4AzimuthAutonPos = (139.6 / 180) * 2048;

// azimuth positions used when starting at 0 azimuth
// const double kWheel1AzimuthAutonPos = (-40.4 / 180) * 2048;
// const double kWheel2AzimuthAutonPos = (40.4 / 180) * 2048;
// const double kWheel3AzimuthAutonPos = (-40.4 / 180) * 2048;
// const double kWheel4AzimuthAutonPos = (40.4 / 180) * 2048;
}

/**
 * This is used to position the wheel azimuths for autonomous rotation of the
 * robot. It moves each wheel a minimum angle by assuming the wheel azimuths
 * started at 2048.
 *
 * @see IsPositionAzimuthForAutonDone(), DriveAzimuthAutonomous(double setpoint)
 */
void SwerveDrive::PositionAzimuthForAuton() {
  map_->rf_azimuth->Set(kWheel2AzimuthAutonPos);  // 2-rf
  map_->lf_azimuth->Set(kWheel1AzimuthAutonPos);  // 1-lf
  map_->lr_azimuth->Set(kWheel4AzimuthAutonPos);  // 4-rr
  map_->rr_azimuth->Set(kWheel3AzimuthAutonPos);  // 3-lr
}

/**
 * Commands use this to determine when wheels are finished moving to position
 * for autonomous rotation.
 *
 * @return true when wheels are in position set by
 * PositionAzimuthForAuton()
 *
 * @see PositionAzimuthForAuton(), DriveAzimuthAutonomous(double setpoint)
 */
bool SwerveDrive::IsPositionAzimuthForAutonDone() {
  int error = std::abs(GetAzimuth(swerve::SwerveDrive::kLeftFront) -
                       kWheel1AzimuthAutonPos);
  SPDLOG_DEBUG(
      logger_,
      "GetPosition(kLeftFront) = {}, kWheel2AzimuthAutonPos = {}, error = {}",
      GetAzimuth(swerve::SwerveDrive::kLeftFront), kWheel1AzimuthAutonPos,
      error);
  return error < 200;
}

/**
 * Drives the azimuth motors that are set in PositionAzimuthForAuton above.
 *
 * Wheels 1 and 4 are driven reverse, while wheels 2 and 3 are driven forward to
 * match the wheel azimuth positions set by
 * PositionAzimuthForAuton()
 *
 * @param setpoint passed directly to Talon, positive is clockwise
 *
 * @see PositionAzimuthForAuton(), IsPositionAzimuthForAutonDone()
 */
void SwerveDrive::DriveAzimuthAutonomous(double setpoint) {
  map_->rf_drive->Set(setpoint);
  map_->lf_drive->Set(-setpoint);
  map_->lr_drive->Set(-setpoint);
  map_->rr_drive->Set(setpoint);
}

/**
 * Autonomous drives without gyro and without dead zone.
 */
void SwerveDrive::DriveAutonomous(double forward, double strafe,
                                  double azimuth) {
  swerve::SwerveDrive::Drive_(forward, strafe, azimuth, 0.05);
}

/**
 * Reset all drive motor encoder counts to zero. Typically, this is done to
 * prepare for an autonomous drive.
 */
void SwerveDrive::ZeroDistance() {
  // TODO: a better name is ZeroDrivePostion
  map_->lf_drive->SetPosition(0);
  map_->rf_drive->SetPosition(0);
  map_->lr_drive->SetPosition(0);
  map_->rr_drive->SetPosition(0);
}

/**
 * Drive the specified number of encoder ticks using motion profile.
 */
void SwerveDrive::DriveDistance(int distance, int azimuth) {
  // TODO: this now duplicates crab auton drive, need to unify and clean up
  // naming. Change distance to setpoint.
  distance_ = distance;
  map_->lf_azimuth->Set(azimuth);
  map_->rf_azimuth->Set(azimuth);
  map_->lr_azimuth->Set(azimuth);
  map_->rr_azimuth->Set(azimuth);

  map_->lf_drive->Set(distance);
  map_->rf_drive->Set(distance);
  map_->lr_drive->Set(distance);
  map_->rr_drive->Set(distance);
}

/**
 * Returns true if motion profile has completed running.
 */
bool SwerveDrive::IsMotionDone() {
  auto pos = GetPosition();
  SPDLOG_DEBUG(logger_, "magic motion distance = {}", pos);
  if (distance_ > 0) {
    return pos >= distance_;
  }
  return pos <= distance_;
}

/**
 * Initializes DriveTeleOp as the default command for this subsystem.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new command::drive::DriveTeleOp());
}

/**
 * Load settings from the cpptoml configuration object.
 */
void SwerveDrive::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config) {
  // load sidewinder configuration from common config file
  if (!config) {
    throw std::invalid_argument("config must not be null");
  }
  auto settings = config->get_table("SIDEWINDER");
  if (!settings) {
    throw std::invalid_argument("SIDEWINDER config is missing");
  }

  // configure logging for this class

  // get swerve drive auton configuration from sidewinder settings
  auto swerve_settings = settings->get_table("SWERVE");
  if (!swerve_settings) {
    throw std::invalid_argument("SIDEWINDER SWERVE config is missing");
  }

  drive_settings_ = talon::Settings::Create(swerve_settings, "drive");
  // SPDLOG_DEBUG(logger_, "dumping drive talon configuration");
  // drive_settings_->LogConfig(logger_);

  drive_auton_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton");
  // SPDLOG_DEBUG(logger_, "dumping drive auton talon configuration");
  // drive_auton_settings_->LogConfig(logger_);

  drive_motion_magic_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton_motion_magic");
  // SPDLOG_DEBUG(logger_, "dumping drive auton motion magic talon
  // configuration"); drive_motion_magic_settings_->LogConfig(logger_);

  drive_azimuth_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton_slow_azimuth");
  // SPDLOG_DEBUG(logger_, "dumping drive azimuth talon configuration");
  // drive_azimuth_settings_->LogConfig(logger_);
}
