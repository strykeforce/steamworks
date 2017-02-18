#include "elevation.h"

#include "robot.h"

using namespace steamworks::command;

/**
 * SetShooterElevation sets elevation to given encoder position. It should be
 * in the range 0-1600
 */
SetShooterElevation::SetShooterElevation(unsigned elevation)
    : frc::InstantCommand("SetShooterElevation"), elevation_(elevation) {
  Requires(Robot::shooter);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetShooterElevation::Initialize() {
  Robot::shooter->SetElevation(elevation_);
}

/**
 * IncrementShooterElevation raises elevation by a small number of encoder
 * ticks.
 */
IncrementShooterElevation::IncrementShooterElevation()
    : frc::InstantCommand("IncrementShooterElevation") {
  Requires(Robot::shooter);
}

/**
 * Sends increment elevation command to the subsystem.
 */
void IncrementShooterElevation::Initialize() {
  Robot::shooter->IncrementElevation();
}

/**
 * DecrementShooterElevation lowers elevation by a small number of encoder
 * ticks.
 */
DecrementShooterElevation::DecrementShooterElevation()
    : frc::InstantCommand("DecrementShooterElevation") {
  Requires(Robot::shooter);
}

/**
 * Sends decrement elevation command to the subsystem.
 */
void DecrementShooterElevation::Initialize() {
  Robot::shooter->DecrementElevation();
}
