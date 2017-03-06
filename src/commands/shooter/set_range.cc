#include "set_range.h"

#include "robot.h"

using namespace steamworks::command;

namespace {
const double kTimeout = 1.5;
const int kElevationGoodEnough = 5;
const int kSpeedGoodEnough = 25;
}

/**
 * SetRange sets elevation to given encoder position. It should be
 * in the range 0-1600
 */
SetRange::SetRange() : frc::Command("SetRange", kTimeout) {
  Requires(Robot::shooter);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetRange::Initialize() {
  elevation_ = Robot::deadeye->GetShooterElevation();
  speed_ = Robot::deadeye->GetShooterSpeed();
  Robot::shooter->SetElevation(elevation_);
  Robot::shooter->SetSpeed(speed_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetRange::IsFinished() {
  int elevation_error = std::abs(elevation_ - Robot::shooter->GetElevation());
  int speed_error = std::abs(speed_ - Robot::shooter->GetSpeed());
  return (elevation_error < kElevationGoodEnough &&
          speed_error < kSpeedGoodEnough) ||
         IsTimedOut();
}
