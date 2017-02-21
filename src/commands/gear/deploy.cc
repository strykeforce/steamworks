#include "deploy.h"

#include "robot.h"

using namespace steamworks::command::gear;

DeployGear::DeployGear() : frc::Command("DeployGear") {
  Requires(Robot::gear_loader);
}

void DeployGear::Initialize() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(false);
  Robot::gear_loader->Deploy();
}

bool DeployGear::IsFinished() {
  return !Robot::gear_loader->IsLimitSwitchClosed();
}

void DeployGear::End() {
  Robot::gear_loader->StopLoader();
  Robot::gear_loader->SetLimitSwitchNormallyOpen(true);
}
