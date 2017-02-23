#include "drive.h"

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
}

/**
 * Configure the drive talons with the "drive" settings in SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetTeleOpMode() {
  // FIXME: hard-coded for voltage mode
  drive_scale_factor_ = 12.0;
  drive_settings_->SetMode(map_->lf_drive);
  drive_settings_->SetMode(map_->rf_drive);
  drive_settings_->SetMode(map_->lr_drive);
  drive_settings_->SetMode(map_->rr_drive);
}

/**
 * Configure the drive talons with the "drive_auton" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAutonMode() {
  // FIXME: hard-coded
  drive_scale_factor_ = 600.0;
  drive_auton_settings_->SetMode(map_->lf_drive);
  drive_auton_settings_->SetMode(map_->rf_drive);
  drive_auton_settings_->SetMode(map_->lr_drive);
  drive_auton_settings_->SetMode(map_->rr_drive);
}

/**
 * Drives without gyro.
 */
void SwerveDrive::DriveAutonomous(double forward, double strafe,
                                  double azimuth) {
  logger_->debug("auton forward = {}, strafe = {}, azimuth = {}", forward,
                 strafe, azimuth);
  swerve::SwerveDrive::Drive_(forward, strafe, azimuth);
}

/**
 * Initializes DriveTeleOp as the default command for this subsystem.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new command::DriveTeleOp());
}
