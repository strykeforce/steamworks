#include "load.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::command::gear;

LoadGear::LoadGear() : frc::Command("LoadGear") {
  Requires(Robot::gear_loader);
  SmartDashboard::PutBoolean(kGearLoadedLabel, false);
}

void LoadGear::Initialize() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->ClampShut();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(true);
  Robot::gear_loader->Load();
  SmartDashboard::PutBoolean(kGearLoadedLabel, false);
}

bool LoadGear::IsFinished() {
  return Robot::gear_loader->IsLimitSwitchClosed();
}

void LoadGear::End() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(false);
  SmartDashboard::PutBoolean(kGearLoadedLabel, true);
}
