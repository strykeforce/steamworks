#include "auton.h"

#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command;

/**
 * DriveAutonomous is a WPI command-based command to autonomously drive the
 * Sidewinder swerve drive.
 */
DriveAutonomous::DriveAutonomous(double forward, double strafe, double azimuth,
                                 int distance)
    : frc::Command("DriveAutonomous"),
      initial_pos_(0),
      distance_(distance),
      forward_(forward),
      strafe_(strafe),
      azimuth_(azimuth) {
  Requires(Robot::drive);
}

void DriveAutonomous::Initialize() {
  initial_pos_ = Robot::drive->GetPosition();
}

void DriveAutonomous::Execute() {
  Robot::drive->CartesianDrive(forward_, strafe_, azimuth_);
}

bool DriveAutonomous::IsFinished() {
  int current_pos = Robot::drive->GetPosition();
  return std::abs(current_pos - initial_pos_) > distance_;
}

void DriveAutonomous::End() { Robot::drive->CartesianDrive(0.0, 0.0, 0.0); }
