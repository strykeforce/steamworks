#include "stop_climb.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command::climber;

StopClimb::StopClimb()
    : frc::InstantCommand("StopClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(false);
}

void StopClimb::Initialize() {
  logger_->info("StopClimb stopping climber with position {}",
                Robot::climber->GetPosition());
  Robot::climber->Stop();
}
