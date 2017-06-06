#include "deploy.h"

#include "load.h"
#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::command::gear;

DeployGear::DeployGear() : frc::Command("DeployGear") {
  Requires(Robot::gear_loader);
}

void DeployGear::Initialize() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLoadLimitSwitchNormallyOpen(false);
  Robot::gear_loader->ClampStage();
  Robot::gear_loader->Deploy();
}

bool DeployGear::IsFinished() {
  return !Robot::gear_loader->IsLoadLimitSwitchClosed();
}

void DeployGear::End() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLoadLimitSwitchNormallyOpen(true);

  SmartDashboard::PutBoolean(kGearLoadedLabel, false);
}
