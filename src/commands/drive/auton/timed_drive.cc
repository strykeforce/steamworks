#include "timed_drive.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

namespace {
const double kTimeout = 0.25;
}

TimedDrive::TimedDrive() : frc::Command("TimedDrive", kTimeout) {
  Requires(Robot::drive);
}

/**
 * Initial send the position to the azimuth motors.
 */
void TimedDrive::Initialize() {
  // initial_ = Robot::drive->GetPosition();
  Robot::drive->SetAutonMode();
  Robot::drive->CrabDriveAutonomous(100, 0);
}

/**
 * Azimuth wheels are close enough
 */
bool TimedDrive::IsFinished() {
  return IsTimedOut();
  // int error = std::abs(Robot::drive->GetPosition() - initial_ - distance_);
  // return (error < kElevationGoodEnough);  // || IsTimedOut();
}
