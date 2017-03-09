#include "shooter_led.h"

#include "robot.h"

using namespace steamworks::command::deadeye;

ShooterLED::ShooterLED(bool enabled)
    : frc::InstantCommand("ShooterLED"), enabled_(enabled) {
  // Requires(Robot::deadeye);
}

void ShooterLED::Initialize() {
  Robot::deadeye->SetShooterLightEnabled(enabled_);
}
