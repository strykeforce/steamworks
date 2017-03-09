#include "timed_azimuth.h"

#include "robot.h"

using namespace steamworks::command::drive;

TimedAzimuth::TimedAzimuth(double speed, double timeout)
    : frc::Command("TimedAzimuth", timeout), speed_(speed) {
  Requires(Robot::drive);
}

/**
 * Initial send the position to the azimuth motors.
 */
void TimedAzimuth::Initialize() {
  Robot::drive->SetAutonMode();
  Robot::drive->DriveAutonomous(0, 0, speed_);
}

/**
 * Azimuth wheels are close enough
 */
bool TimedAzimuth::IsFinished() { return IsTimedOut(); }
