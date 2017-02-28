#include "zero.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command;

DriveZero::DriveZero() : frc::Command("DriveZero") { Requires(Robot::drive); }

void DriveZero::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->CrabDriveAutonomous(250.0, 0);
}

bool DriveZero::IsFinished() { return false; }

void DriveZero::End() { Robot::drive->CrabDriveAutonomous(0.0, 0); }
