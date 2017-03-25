#include "brake.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

EnableBrake::EnableBrake() : InstantCommand("EnableBrake") {
  Requires(Robot::drive);
}

void EnableBrake::Initialize() { Robot::drive->SetBrakeEnabled(true); }
