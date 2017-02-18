#include "wheel.h"

#include "robot.h"

using namespace steamworks::command;

namespace {
const std::string kWheelSpeedKey("Wheel Speed");
}

/**
 * StartShooterWheel sets shooter wheel speed to mid-speed.
 */
StartShooterWheel::StartShooterWheel()
    : frc::InstantCommand("StartShooterWheel") {
  Requires(Robot::shooter);
}

/**
 * Sends wheel speed set command to the subsystem.
 */
void StartShooterWheel::Initialize() {
  Robot::shooter->SetSpeed(460);
  SmartDashboard::PutNumber(kWheelSpeedKey, 460);
}

/**
 * StopShooterWheel ramps shooter wheel speed down to zero.
 */
StopShooterWheel::StopShooterWheel() : frc::Command("StopShooterWheel") {
  Requires(Robot::shooter);
}

/**
 * Gets the currently running speed.
 */
void StopShooterWheel::Initialize() {
  speed_setpoint_ = Robot::shooter->GetSpeed();
}

namespace {
const unsigned kSpeedDecrement = 6;
}

/**
 * Performs the speed ramp-down by decrementing a small amount per command
 * execute loop iteration.
 */
void StopShooterWheel::Execute() {
  if (speed_setpoint_ < kSpeedDecrement) {
    speed_setpoint_ = 0;
  } else {
    speed_setpoint_ -= kSpeedDecrement;
  }
  Robot::shooter->SetSpeed(speed_setpoint_);
  SmartDashboard::PutNumber(kWheelSpeedKey, speed_setpoint_);
}

bool StopShooterWheel::IsFinished() { return speed_setpoint_ == 0; }

/**
 * DecrementShooterSpeed lowers wheel speed by a small number of encoder
 * ticks.
 */
IncrementShooterSpeed::IncrementShooterSpeed()
    : frc::InstantCommand("IncrementShooterSpeed") {
  Requires(Robot::shooter);
}

/**
 * Sends increment wheel speed command to the subsystem.
 */
void IncrementShooterSpeed::Initialize() {
  Robot::shooter->IncrementSpeed();
  SmartDashboard::PutNumber(kWheelSpeedKey, Robot::shooter->GetSpeedSetpoint());
}

/**
 * IncrementShooterSpeed lowers wheel speed by a small number of encoder
 * ticks.
 */
DecrementShooterSpeed::DecrementShooterSpeed()
    : frc::InstantCommand("DecrementShooterSpeed") {
  Requires(Robot::shooter);
}

/**
 * Sends decrement wheel speed command to the subsystem.
 */
void DecrementShooterSpeed::Initialize() {
  Robot::shooter->DecrementSpeed();
  SmartDashboard::PutNumber(kWheelSpeedKey, Robot::shooter->GetSpeedSetpoint());
}
