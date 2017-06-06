#include "elevation.h"

#include "robot.h"
#include "subsystems/shooter_elevation.h"

using namespace steamworks::command;

namespace {
const double kTimeout = 1.5;
const int kGoodEnough = 5;
}

/**
 * SetShooterElevation sets elevation to given encoder position. It should be
 * in the range 0-1600
 */
SetShooterElevation::SetShooterElevation(unsigned elevation)
    : frc::Command("SetShooterElevation", kTimeout), elevation_(elevation) {
  Requires(Robot::shooter_elevation);
}

/**
 * Sends elevation set command to the subsystem.
 */
void SetShooterElevation::Initialize() {
  Robot::shooter_elevation->SetElevation(elevation_);
}

/**
 * Returns true when elevation position is reached.
 */
bool SetShooterElevation::IsFinished() {
  int error = std::abs(elevation_ - Robot::shooter_elevation->GetElevation());
  return error < kGoodEnough || IsTimedOut();
}

//
// IncrementShooterElevation
//
/**
 * IncrementShooterElevation raises elevation by a small number of encoder
 * ticks.
 */
IncrementShooterElevation::IncrementShooterElevation()
    : frc::InstantCommand("IncrementShooterElevation") {
  Requires(Robot::shooter_elevation);
}

/**
 * Sends increment elevation command to the subsystem.
 */
void IncrementShooterElevation::Initialize() {
  Robot::shooter_elevation->IncrementElevation();
}

/**
 * DecrementShooterElevation lowers elevation by a small number of encoder
 * ticks.
 */
DecrementShooterElevation::DecrementShooterElevation()
    : frc::InstantCommand("DecrementShooterElevation") {
  Requires(Robot::shooter_elevation);
}

/**
 * Sends decrement elevation command to the subsystem.
 */
void DecrementShooterElevation::Initialize() {
  Robot::shooter_elevation->DecrementElevation();
}
