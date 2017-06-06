#include "drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "commands/tele_drive.h"
#include "robot_map.h"
#include "sidewinder/swerve/swerve_drive.h"

using namespace avenger;
using namespace sidewinder;

AvengerDrive::AvengerDrive(const std::shared_ptr<cpptoml::table> config)
    : swerve::SwerveDrive(config, RobotMap::swerve_talons, RobotMap::gyro) {}

void AvengerDrive::InitDefaultCommand() { SetDefaultCommand(new TeleDrive()); }
