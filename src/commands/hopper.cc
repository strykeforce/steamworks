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
  logger_->debug("starting hopper");
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
  logger_->debug("stopping hopper");
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
