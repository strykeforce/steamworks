#include "gear_intake.h"

#include "config.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::trigger;
using namespace frc;

GearIntake::GearIntake() : Trigger() {}

bool GearIntake::Get() {
#ifndef NO_GEAR
  return Robot::gear_loader->IsIntakeLimitSwitchClosed();
#else
  return false;
#endif
}
