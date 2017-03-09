#include "timed_swerve_drive.h"

#include "robot.h"

using namespace steamworks::command::drive;

TimedSwerveDrive::TimedSwerveDrive(double forward, double strafe,
                                   double timeout)
    : frc::Command("TimedSwerveDrive", timeout),
      forward_(forward),
      strafe_(strafe) {
  Requires(Robot::drive);
}

/**
 * Initial send the position to the azimuth motors.
 */
void TimedSwerveDrive::Initialize() {
  // initial_ = Robot::drive->GetPosition();
  Robot::drive->SetGyroDisabled(true);
  Robot::drive->SetAutonMode();
  Robot::drive->Drive(forward_, strafe_, 0);
}

/**
 * Azimuth wheels are close enough
 */
bool TimedSwerveDrive::IsFinished() {
  return IsTimedOut();
  // int error = std::abs(Robot::drive->GetPosition() - initial_ - distance_);
  // return (error < kElevationGoodEnough);  // || IsTimedOut();
}

void TimedSwerveDrive::End() { Robot::drive->SetGyroDisabled(false); }
