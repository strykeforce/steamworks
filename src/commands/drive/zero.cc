#include "zero.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command;

DriveZero::DriveZero() : frc::Command("DriveZero") { Requires(Robot::drive); }

void DriveZero::Initialize() {
  auto st = RobotMap::swerve_talons;

  st->lf_azimuth->Set(0.0);
  st->rf_azimuth->Set(0.0);
  st->lr_azimuth->Set(0.0);
  st->rr_azimuth->Set(0.0);

  st->lf_drive->Set(3.0);
  st->rf_drive->Set(3.0);
  st->lr_drive->Set(3.0);
  st->rr_drive->Set(3.0);
}

bool DriveZero::IsFinished() { return false; }

void DriveZero::End() {
  auto st = RobotMap::swerve_talons;

  st->lf_azimuth->StopMotor();
  st->rf_azimuth->StopMotor();
  st->lr_azimuth->StopMotor();
  st->rr_azimuth->StopMotor();

  st->lf_drive->StopMotor();
  st->rf_drive->StopMotor();
  st->lr_drive->StopMotor();
  st->rr_drive->StopMotor();
}
