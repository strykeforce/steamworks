#include "drive.h"

#include "commands/drive/tele_op.h"
#include "motion.h"
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
  assert(drive_settings_);
  SetDriveMode(drive_settings_, drive_settings_->GetSetpointMax());
}

/**
 * Configure the drive talons with the "drive_auton" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAutonMode() {
  assert(drive_settings_);
  SetDriveMode(drive_auton_settings_, drive_auton_settings_->GetSetpointMax());
}

/**
 * Configure the drive talons with the "drive_auton_slow_azimuth" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAzimuthMode() {
  assert(drive_settings_);
  SetDriveMode(drive_auton_settings_, drive_auton_settings_->GetSetpointMax());
}

/**
 * Configure the drive talons with the "drive_auton_motion_magic" settings in
 * SIDEWINDER SWERVE.
 */
void SwerveDrive::SetMotionMagicMode() {
  assert(drive_motion_magic_settings_);
  SetDriveMode(drive_motion_magic_settings_, 0);
}

/**
 * Autonomous drives without gyro and without dead zone.
 */
void SwerveDrive::DriveAutonomous(double forward, double strafe,
                                  double azimuth) {
  swerve::SwerveDrive::Drive_(forward, strafe, azimuth, 0);
}

/**
 * Zero out the drive encoders.
 */
void SwerveDrive::ZeroDistance() {
  map_->lf_drive->SetPosition(0);
  map_->rf_drive->SetPosition(0);
  map_->lr_drive->SetPosition(0);
  map_->rr_drive->SetPosition(0);
}

/**
 * Drive the specified number of encoder ticks using motion profile.
 */
void SwerveDrive::DriveDistance(int distance, int azimuth) {
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
  logger_->debug("magic motion distance = {}", pos);
  if (distance_ > 0) {
    return pos >= distance_;
  }
  return pos <= distance_;
}

/**
 * Initializes DriveTeleOp as the default command for this subsystem.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new command::DriveTeleOp());
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
  logger_->debug("dumping drive talon configuration");
  drive_settings_->LogConfig(logger_);

  drive_auton_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton");
  logger_->debug("dumping drive auton talon configuration");
  drive_auton_settings_->LogConfig(logger_);

  drive_motion_magic_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton_motion_magic");
  logger_->debug("dumping drive auton motion magic talon configuration");
  drive_motion_magic_settings_->LogConfig(logger_);

  drive_azimuth_settings_ =
      talon::Settings::Create(swerve_settings, "drive_auton_slow_azimuth");
  logger_->debug("dumping drive azimuth talon configuration");
  drive_azimuth_settings_->LogConfig(logger_);
}
