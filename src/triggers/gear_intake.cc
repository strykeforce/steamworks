#include "gear_intake.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::trigger;
using namespace frc;

GearIntake::GearIntake() : Trigger() {}

bool GearIntake::Get() {
  return Robot::gear_loader->IsIntakeLimitSwitchClosed();
}
