#include "finish_climb.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command::climber;

FinishClimb::FinishClimb()
    : frc::InstantCommand("FinishClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

/**
 * FinishClimb
 */
void FinishClimb::Initialize() {
  SPDLOG_DEBUG(logger_, "FinishClimb initialized with position {}",
               Robot::climber->GetPosition());
  Robot::climber->SetFinishModeEnabled(true);
  Robot::climber->StartFinish();
}
