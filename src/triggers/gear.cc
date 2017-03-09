#include "gear.h"

#include "oi.h"

using namespace steamworks::trigger;
using namespace frc;

namespace {
const double kTriggerActive = 0.5;
}

Gear::Gear() : Trigger(), gamepad_(OI::kGamePadJoystick) {}

bool Gear::Get() {
  return gamepad_.GetTriggerAxis(XboxController::kLeftHand) > kTriggerActive;
}
