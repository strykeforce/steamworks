#include "hopper.h"

#include "robot.h"
#include "subsystems/hopper.h"
#include "subsystems/deadeye.h"
#include "robot.h"

using namespace steamworks::command;

//
// StartHopper
//

/**
 * Designated initializer.
 */
StartHopper::StartHopper(double voltage)
    : frc::InstantCommand("StartHopper"),
      logger_(spdlog::get("command")),
      voltage_(voltage) {
  Requires(Robot::hopper);
}

/**
 * Initialize with config file voltage.
 */
StartHopper::StartHopper() : StartHopper(0) {}

/**
 * Start hopper.
 */
void StartHopper::Initialize() {
  if (voltage_ != 0) {
    Robot::hopper->Start(voltage_);
    return;
  }
  auto v = Robot::deadeye->GetSolutionHopperVoltage();
  Robot::hopper->Start(v);  // use config default
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

namespace {
  double kVoltageIncrement = 0.1;
}
IncrementHopperVoltage::IncrementHopperVoltage()
    : frc::InstantCommand("IncrementHopperVoltage") {
  Requires(Robot::hopper);
}

void IncrementHopperVoltage::Initialize() {
  double voltage = Robot::hopper->GetVoltage() + kVoltageIncrement;
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
  double voltage = Robot::hopper->GetVoltage() - kVoltageIncrement;
  if (voltage < 0.0) {
    voltage = 0.0;
  }
  Robot::hopper->SetVoltage(voltage);
}
