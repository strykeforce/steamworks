#include "drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "commands/tele_drive.h"
#include "robot_map.h"
#include "swerve/swerve_drive.h"

using namespace sidewinder;

Drive::Drive(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Drive"),
      gyro_(std::make_shared<AHRS>(SPI::Port::kMXP)),
      swerve_drive_(config, RobotMap::swerve_talons, gyro_) {
  gyro_->ZeroYaw();
}

void Drive::CartesianDrive(double forward, double strafe, double azimuth) {
  swerve_drive_.Drive(forward, strafe, azimuth);
}

void Drive::ZeroWheels() { swerve_drive_.ZeroAzimuth(); }

void Drive::InitDefaultCommand() { SetDefaultCommand(new TeleDrive()); }
