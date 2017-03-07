#include "set_elevation.h"

#include "robot.h"

using namespace steamworks::command::shooter;

namespace {
const double kTimeout = 2.0;
const int kElevationGoodEnough = 10;
}

/**
 * SetElevation sets shooter elevation to shooting solution elevation.
 */
SetElevation::SetElevation() : frc::Command("SetElevation", kTimeout) {
  // Requires(Robot::shooter);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetElevation::Initialize() {
  elevation_ = Robot::shooter->GetSolutionElevation();
  Robot::shooter->SetElevation(elevation_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetElevation::IsFinished() {
  int elevation_error = std::abs(elevation_ - Robot::shooter->GetElevation());
  return (elevation_error < kElevationGoodEnough) || IsTimedOut();
}
