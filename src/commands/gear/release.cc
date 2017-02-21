#include "release.h"

#include "robot.h"

using namespace steamworks::command::gear;

ReleaseGear::ReleaseGear() : frc::Command("ReleaseGear") {
  Requires(Robot::gear_loader);
}

/**
* Initialize
*/
void ReleaseGear::Initialize() {
  // FIXME: hardcoded
  Robot::gear_loader->SetServo(1.0);
  Robot::gear_loader->PivotDown();
}

/**
 * IsFinished
 */
bool ReleaseGear::IsFinished() {
  return Robot::gear_loader->GetPivotPosition() < 10;
}
