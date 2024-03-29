#include "set_elevation.h"

#include "robot.h"
#include "subsystems/deadeye.h"
#include "subsystems/shooter_elevation.h"

using namespace steamworks::command::shooter;

namespace {
const int kElevationGoodEnough = 10;
}

/**
 * SetElevation sets shooter elevation to shooting solution elevation.
 */
SetElevation::SetElevation()
    : frc::Command("SetElevation"), logger_(spdlog::get("command")) {
  Requires(Robot::shooter_elevation);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetElevation::Initialize() {
  elevation_ = Robot::deadeye->GetSolutionElevation();
  Robot::shooter_elevation->SetElevation(elevation_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetElevation::IsFinished() {
  int elevation_error =
      std::abs(elevation_ - Robot::shooter_elevation->GetElevation());
  return (elevation_error < kElevationGoodEnough) || IsTimedOut();
}

/**
 * Tell us when elevation is reached
 */
void SetElevation::End() {
  logger_->info("SetElevation ended with setpoint {} and actual {}", elevation_,
                Robot::shooter_elevation->GetElevation());
}
