#include "clear.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::command::gear;

StartClear::StartClear() : frc::InstantCommand("StartClear") {
  Requires(Robot::gear_loader);
}

void StartClear::Initialize() { Robot::gear_loader->ClearLoader(); }

StopClear::StopClear() : frc::InstantCommand("StopClear") {
  Requires(Robot::gear_loader);
}

void StopClear::Initialize() { Robot::gear_loader->StopLoader(); }
