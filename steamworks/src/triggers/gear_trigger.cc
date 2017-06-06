#include "gear_trigger.h"

#include "oi.h"

using namespace steamworks::trigger;
using namespace frc;

namespace {
const double kTriggerActive = 0.5;
}

GearTrigger::GearTrigger() : Trigger(), gamepad_(OI::kGamePadJoystick) {}

bool GearTrigger::Get() {
  return gamepad_.GetTriggerAxis(XboxController::kLeftHand) > kTriggerActive;
}
