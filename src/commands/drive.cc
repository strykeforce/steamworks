#include "drive.h"

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "commands/log.h"
#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command;

DriveTele::DriveTele()
    : frc::Command("DriveTele"), logger_(spdlog::get("command")) {
  Requires(Robot::drive);
}

void DriveTele::Execute() {
  double forward = Robot::oi->GetTeleDriveForwardAxis();
  double strafe = Robot::oi->GetTeleDriveStrafeAxis();
  double azimuth = Robot::oi->GetTeleDriveAzimuthAxis();
  Robot::drive->CartesianDrive(forward, strafe, azimuth);
}

bool DriveTele::IsFinished() { return false; }

void DriveTele::End() { Robot::drive->CartesianDrive(0.0, 0.0, 0.0); }

//
// DriveFieldRelative
//
DriveFieldRelative::DriveFieldRelative(Direction direction, double timeout)
    : frc::TimedCommand("DriveFieldRelative", timeout),
      logger_(spdlog::get("command")),
      direction_(direction) {
  Requires(Robot::drive);
}

void DriveFieldRelative::Execute() {
  double forward = 0.0, strafe = 0.0, azimuth = 0.0;
  switch (direction_) {
    case kUp:
      forward = 0.2;
      break;
    case kDown:
      forward = -0.2;
      break;
    case kLeft:
      strafe = -0.2;
      break;
    case kRight:
      strafe = 0.2;
      break;
  }
  logger_->debug("position = {}", Robot::drive->GetPosition());
  Robot::drive->CartesianDrive(forward, strafe, azimuth);
}

void DriveFieldRelative::End() { Robot::drive->CartesianDrive(0.0, 0.0, 0.0); }

//
// DriveClockwise
//
DriveClockwise::DriveClockwise(double timeout)
    : frc::TimedCommand("DriveClockwise", timeout),
      logger_(spdlog::get("command")) {
  Requires(Robot::drive);
}

void DriveClockwise::Execute() {
  logger_->warn(__PRETTY_FUNCTION__);
  Robot::drive->CartesianDrive(0.0, 0.0, 0.2);
}

void DriveClockwise::End() { Robot::drive->CartesianDrive(0.0, 0.0, 0.0); }

//
// DriveSquare
//

DriveSquare::DriveSquare() : frc::CommandGroup("DriveSquare") {
  AddSequential(new Log("DriveFieldRelative Up 2.0"));
  AddSequential(new DriveFieldRelative(DriveFieldRelative::kUp, 2.0));
  // AddSequential(new Log("DriveClockwise 2.0"));
  // AddSequential(new DriveClockwise(2.0));
  AddSequential(new Log("DriveFieldRelative Right 2.0"));
  AddSequential(new DriveFieldRelative(DriveFieldRelative::kRight, 2.0));

  AddSequential(new Log("DriveFieldRelative Down 2.0"));
  AddSequential(new DriveFieldRelative(DriveFieldRelative::kDown, 2.0));

  AddSequential(new Log("DriveFieldRelative Left 2.0"));
  AddSequential(new DriveFieldRelative(DriveFieldRelative::kLeft, 2.0));

  AddSequential(new Log("DriveClockwise 1.0"));
  AddSequential(new DriveClockwise(1.0));
}
