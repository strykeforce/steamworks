#include "set_zero.h"

#include "ctrlib/CANTalon.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

//
// WriteAzimuthCalibration
//
WriteAzimuthCalibration::WriteAzimuthCalibration()
    : frc::InstantCommand("WriteAzimuthCalibration") {
  Requires(Robot::drive);
}

void WriteAzimuthCalibration::Initialize() {
  Robot::drive->WriteAzimuthCalibration();
  read_cal_.Start();
}

//
// ReadAzimuthCalibration
//
ReadAzimuthCalibration::ReadAzimuthCalibration()
    : frc::InstantCommand("ReadAzimuthCalibration") {
  Requires(Robot::drive);
}

void ReadAzimuthCalibration::Initialize() {
  Robot::drive->ReadAzimuthCalibration();
}

//
// ZeroWheelAzimuth
//
ZeroWheelAzimuth::ZeroWheelAzimuth() : frc::Command("ZeroWheelAzimuth") {
  Requires(Robot::drive);
}

void ZeroWheelAzimuth::Initialize() { Robot::drive->ZeroAzimuth(); }

bool ZeroWheelAzimuth::IsFinished() {
  // return false;
  return std::abs(Robot::drive->GetAzimuth()) < 3;
}

void ZeroWheelAzimuth::End() {
  // take azimuth motors out of closed-loop to allow zero adjustments
  RobotMap::swerve_talons->lf_azimuth->StopMotor();
  RobotMap::swerve_talons->rf_azimuth->StopMotor();
  RobotMap::swerve_talons->lr_azimuth->StopMotor();
  RobotMap::swerve_talons->rr_azimuth->StopMotor();
}
