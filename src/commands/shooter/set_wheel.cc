#include "set_wheel.h"

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/shooter_wheel.h"

using namespace steamworks::command::shooter;

namespace {
const int kSpeedGoodEnough = 5;
}

/**
 * SetWheel sets wheel speed to shooting solution speed.
 */
SetWheel::SetWheel()
    : frc::Command("SetWheel"), logger_(spdlog::get("command")) {
  Requires(Robot::shooter_wheel);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetWheel::Initialize() {
  speed_ = Robot::deadeye->GetSolutionWheelSpeed();
  Robot::shooter_wheel->SetSpeed(speed_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetWheel::IsFinished() {
  int speed_error = std::abs(speed_ - Robot::shooter_wheel->GetSpeed());
  return (speed_error < kSpeedGoodEnough) || IsTimedOut();
}

/**
 * Tell us when wheel is up to speed.
 */
void SetWheel::End() {
  logger_->info("SetWheel ended with setpoint {} and actual {}", speed_,
                Robot::shooter_wheel->GetSpeed());
}
