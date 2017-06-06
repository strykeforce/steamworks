#include "start_climb.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command::climber;

StartClimb::StartClimb()
    : frc::Command("StartClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

/**
 * Start the climb
 */
void StartClimb::Initialize() {
  logger_->info("StartClimb starting climber with position {}",
                Robot::climber->GetPosition());
  Robot::climber->StartClimb();
}

/**
 * Finished when we have traveled desired distance up rope.
 */
bool StartClimb::IsFinished() { return Robot::climber->ShouldClimbFinish(); }

/**
 * Button released
 */
void StartClimb::Interrupted() {
  SPDLOG_DEBUG(logger_, "StartClimb climb is interrupted");
}

/**
 * Done with climb
 */
void StartClimb::End() {
  logger_->info("StartClimb finished rope climb");
  finish_.Start();
}
