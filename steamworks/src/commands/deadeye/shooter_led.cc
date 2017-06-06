#include "shooter_led.h"

#include "robot.h"
#include "subsystems/deadeye.h"

using namespace steamworks::command::deadeye;

ShooterLED::ShooterLED(bool enabled)
    : frc::InstantCommand("ShooterLED"), enabled_(enabled) {
  // Requires(Robot::deadeye);
}

void ShooterLED::Initialize() {
  Robot::deadeye->SetShooterLightEnabled(enabled_);
}
