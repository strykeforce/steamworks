#include "drive.h"

#include "commands/drive/tele_op.h"
#include "robot_map.h"

using namespace steamworks::subsystem;
namespace sws = sidewinder::swerve;

/**
 * SwerveDrive is the drive subsystem and sets DriveTeleOp as its default
 * command.
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config)
    : sws::SwerveDrive(config, RobotMap::swerve_talons, RobotMap::gyro),
      logger_(spdlog::get("subsystem")) {
  // swerve_drive_.SetGyroDisabled(true);
}

/**
 * Initializes DriveTeleOp as the default command for this subsystem.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new command::DriveTeleOp());
}
