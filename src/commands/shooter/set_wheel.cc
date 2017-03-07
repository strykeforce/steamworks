#include "set_wheel.h"

#include "robot.h"

using namespace steamworks::command::shooter;

namespace {
const double kTimeout = 2.0;
const int kSpeedGoodEnough = 5;
}

/**
 * SetWheel sets wheel speed to shooting solution speed.
 */
SetWheel::SetWheel() : frc::Command("SetWheel", kTimeout) {
  // Requires(Robot::shooter);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetWheel::Initialize() {
  speed_ = Robot::shooter->GetSolutionWheelSpeed();
  Robot::shooter->SetSpeed(speed_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetWheel::IsFinished() {
  int speed_error = std::abs(speed_ - Robot::shooter->GetSpeed());
  return (speed_error < kSpeedGoodEnough) || IsTimedOut();
}
