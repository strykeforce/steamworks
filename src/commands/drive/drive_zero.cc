#include "drive_zero.h"

#include "robot.h"
#include "robot_map.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

namespace {
const double kDriveSetpoint = 250.0;
}

/**
 * This command is used for aligning the wheels.
 */
DriveZero::DriveZero() : frc::Command("DriveZero") { Requires(Robot::drive); }

void DriveZero::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->CrabDriveAutonomous(kDriveSetpoint, 0);
}

bool DriveZero::IsFinished() { return false; }

void DriveZero::End() { Robot::drive->CrabDriveAutonomous(0.0, 0); }
