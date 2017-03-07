#include "pivot.h"

#include "robot.h"

using namespace steamworks::command::gear;

PivotGear::PivotGear() : frc::Command("PivotGear") {
  Requires(Robot::gear_loader);
}

void PivotGear::Initialize() { Robot::gear_loader->PivotUp(); }

bool PivotGear::IsFinished() { return Robot::gear_loader->IsPivotUp(); }
