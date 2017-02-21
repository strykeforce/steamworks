#include "load.h"

#include "robot.h"

using namespace steamworks::command::gear;

//
// ReadyLoadGear
//
ReadyLoadGear::ReadyLoadGear() : frc::Command("ReadyLoadGear") {
  Requires(Robot::gear_loader);
}

void ReadyLoadGear::Initialize() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(true);
}

bool ReadyLoadGear::IsFinished() {
  return Robot::gear_loader->IsIntakeEnabled();
}

//
// LoadGear
//
LoadGear::LoadGear() : frc::Command("LoadGear") {
  Requires(Robot::gear_loader);
}

void LoadGear::Initialize() {
  Robot::gear_loader->StopLoader();
  // FIXME: hardcoded
  Robot::gear_loader->SetServo(0.7);
  Robot::gear_loader->SetLimitSwitchNormallyOpen(true);
  Robot::gear_loader->Load();
}

bool LoadGear::IsFinished() {
  return Robot::gear_loader->IsLimitSwitchClosed();
}

void LoadGear::End() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(false);
}
