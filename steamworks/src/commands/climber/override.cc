#include "override.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command::climber;

Override::Override()
    : frc::InstantCommand("Override"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(false);
}

void Override::Initialize() {
  logger_->info("Override stopping climber with position {}",
                Robot::climber->GetPosition());
  Robot::climber->StartClimb();
}
