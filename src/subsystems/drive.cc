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
  // FIXME: hard-coded for voltage mode
  assert(drive_settings_);
  SetDriveMode(drive_settings_, 12.0);
}

/**
 * Configure the drive talons with the "drive_auton" settings in
 * SIDEWINDER.SWERVE.
 */
void SwerveDrive::SetAutonMode() {
  // FIXME: hard-coded
  SetDriveMode(drive_auton_settings_, 600.0);
}

/**
 * Autonomous drives without gyro.
 */
void SwerveDrive::DriveAutonomous(double forward, double strafe,
                                  double azimuth) {
  swerve::SwerveDrive::Drive_(forward, strafe, azimuth);
}

namespace {
const unsigned kMaxV = 400;
const unsigned kTimeV = 400;
const unsigned kTimeA = 200;
const unsigned kPeriod = 25;
}
/**
 * Drive the specified number of encoder ticks using motion profile.
 */
void SwerveDrive::DriveDistance(int distance) {
  motion_ =
      std::make_unique<Motion>(this, distance, kMaxV, kTimeV, kTimeA, kPeriod);
}

/**
 * Returns true if motion profile has completed running.
 */
bool SwerveDrive::IsMotionDone() {
  bool done = motion_->OnTarget();
  if (done) motion_.reset(nullptr);
  return done;
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
}
