#include "hopper.h"

#include "robot.h"
#include "subsystems/hopper.h"

using namespace steamworks::command;

Hopper::Hopper(Operation operation)
    : frc::InstantCommand("Hopper"),
      logger_(spdlog::get("command")),
      operation_(operation),
      is_started_(false) {
  Requires(Robot::hopper);
}

void Hopper::Initialize() {
  switch (operation_) {
    case kStart:
      Start();
      break;
    case kStop:
      Stop();
      break;
    case kToggle:
      Toggle();
      break;
  }
}

void Hopper::Start() {
  logger_->debug("starting hopper");
  Robot::hopper->Start();
  is_started_ = true;
}

void Hopper::Stop() {
  logger_->debug("stopping hopper");
  Robot::hopper->Stop();
  is_started_ = false;
}

void Hopper::Toggle() {
  if (is_started_) {
    Stop();
    return;
  }
  Start();
}
