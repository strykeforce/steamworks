#include "drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "commands/drive.h"
#include "robot_map.h"
#include "sidewinder/swerve/gyro_swerve_drive.h"

using namespace steamworks::subsystem;
using namespace sidewinder::swerve;

Drive::Drive(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Drive"),
      logger_(spdlog::get("subsystem")),
      gyro_(std::make_shared<AHRS>(SPI::Port::kMXP)),
      swerve_drive_(config, RobotMap::swerve_talons, gyro_) {
  // swerve_drive_.SetGyroDisabled(true);
}

void Drive::CartesianDrive(float forward, float strafe, float azimuth) {
  swerve_drive_.Drive(forward, strafe, azimuth);
}

void Drive::ZeroWheels() { swerve_drive_.ZeroAzimuth(); }

void Drive::InitDefaultCommand() {
  SetDefaultCommand(new command::DriveTele());
}

int Drive::GetPosition(SwerveDrive::Wheel wheel) const {
  return swerve_drive_.GetPosition(wheel);
}

int Drive::GetAzimuth(SwerveDrive::Wheel wheel) const {
  return swerve_drive_.GetAzimuth(wheel);
}

void Drive::WriteAzimuthCalibration() {
  swerve_drive_.WriteAzimuthCalibration();
}
void Drive::ReadAzimuthCalibration() { swerve_drive_.ReadAzimuthCalibration(); }

void Drive::ZeroGyroYaw() { gyro_->ZeroYaw(); }
