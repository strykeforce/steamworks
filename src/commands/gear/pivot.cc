#include "pivot.h"

#include "robot.h"

using namespace steamworks::command::gear;

PivotGear::PivotGear() : frc::Command("PivotGear") {
  Requires(Robot::gear_loader);
}

void PivotGear::Initialize() {
  Robot::gear_loader->SetPivotZeroModeEnabled(false);
  Robot::gear_loader->PivotUp();
}

bool PivotGear::IsFinished() {
  // FIXME: hardcoded
  return Robot::gear_loader->GetPivotPosition() > 1080;
}
