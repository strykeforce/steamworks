#include "brake.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

EnableBrake::EnableBrake() : frc::Command("EnableBrake") {
  Requires(Robot::drive);
}

void EnableBrake::Initialize() { Robot::drive->SetBrake(); }

bool EnableBrake::IsFinished() { return false; }

DisableBrake::DisableBrake() : frc::InstantCommand("DisableBrake") {
  Requires(Robot::drive);  // cancel EnableBrake
}
