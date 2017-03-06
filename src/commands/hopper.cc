#include "hopper.h"

#include "robot.h"
#include "subsystems/hopper.h"

using namespace steamworks::command;

//
// StartHopper
//
StartHopper::StartHopper()
    : frc::InstantCommand("StartHopper"), logger_(spdlog::get("command")) {
  Requires(Robot::hopper);
}

void StartHopper::Initialize() {
  SPDLOG_DEBUG(logger_, "starting hopper");
  Robot::hopper->Start();
}

//
// StopHopper
//
StopHopper::StopHopper()
    : frc::InstantCommand("StopHopper"), logger_(spdlog::get("command")) {
  Requires(Robot::hopper);
}

void StopHopper::Initialize() {
  SPDLOG_DEBUG(logger_, "stopping hopper");
  Robot::hopper->Stop();
}

//
// ToggleHopper
//
ToggleHopper::ToggleHopper()
    : frc::InstantCommand("ToggleHopper"),
      logger_(spdlog::get("command")),
      start_cmd_(),
      stop_cmd_() {
  Requires(Robot::hopper);
}

void ToggleHopper::Initialize() {
  if (Robot::hopper->IsRunning()) {
    stop_cmd_.Start();
    return;
  }
  start_cmd_.Start();
}

//
// IncrementHopperVoltage
//
IncrementHopperVoltage::IncrementHopperVoltage()
    : frc::InstantCommand("IncrementHopperVoltage") {
  Requires(Robot::hopper);
}

void IncrementHopperVoltage::Initialize() {
  double voltage = Robot::hopper->GetVoltage() + 1.0;
  if (voltage > 12.0) {
    voltage = 12.0;
  }
  Robot::hopper->SetVoltage(voltage);
}

//
// DecrementHopperVoltage
//
DecrementHopperVoltage::DecrementHopperVoltage()
    : frc::InstantCommand("DecrementHopperVoltage") {
  Requires(Robot::hopper);
}

void DecrementHopperVoltage::Initialize() {
  double voltage = Robot::hopper->GetVoltage() - 1.0;
  if (voltage < 0.0) {
    voltage = 0.0;
  }
  Robot::hopper->SetVoltage(voltage);
}
