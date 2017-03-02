#include "position_azimuth.h"

#include "robot.h"

using namespace steamworks::command::drive;

PositionAzimuth::PositionAzimuth() : frc::Command("PositionAzimuth") {
  Requires(Robot::drive);
}

/**
 * Initial send the position to the azimuth motors.
 */
void PositionAzimuth::Initialize() { Robot::drive->PositionAzimuthForAuton(); }

/**
 * Azimuth wheels are close enough
 */
bool PositionAzimuth::IsFinished() {
  return Robot::drive->IsPositionAzimuthForAutonDone();
}
