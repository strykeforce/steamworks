#include "zero.h"

#include "robot.h"
#include "robot_map.h"

using namespace steamworks::command::drive;

namespace {
const double kDriveSetpoint = 250.0;
}

Zero::Zero() : frc::Command("Zero") { Requires(Robot::drive); }

void Zero::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->CrabDriveAutonomous(kDriveSetpoint, 0);
}

bool Zero::IsFinished() { return false; }

void Zero::End() { Robot::drive->CrabDriveAutonomous(0.0, 0); }
