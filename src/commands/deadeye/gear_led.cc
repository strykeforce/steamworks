#include "gear_led.h"

#include "robot.h"

using namespace steamworks::command::deadeye;

GearLED::GearLED(bool enabled)
    : frc::InstantCommand("GearLED"), enabled_(enabled) {
  Requires(Robot::deadeye);
}

void GearLED::Initialize() { Robot::deadeye->SetGearLightEnabled(enabled_); }
