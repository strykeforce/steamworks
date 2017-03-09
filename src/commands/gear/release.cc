#include "release.h"

#include "robot.h"

using namespace steamworks::command::gear;

ReleaseGear::ReleaseGear() : frc::Command("ReleaseGear") {
  Requires(Robot::gear_loader);
}

/**
* Release gear on lift peg.
*/
void ReleaseGear::Initialize() {
  Robot::gear_loader->ClampRelease();
  Robot::gear_loader->PivotDown();
}

/**
 * Finished when pivot has returned to lowered position.
 */
bool ReleaseGear::IsFinished() { return Robot::gear_loader->IsPivotDown(); }

/**
 * Stow clamp after releasing.
 */
void ReleaseGear::End() { Robot::gear_loader->ClampShut(); }
