#include "intake.h"

#include "robot.h"
#include "subsystems/intake.h"

using namespace steamworks::command;

Intake::Intake(Operation operation)
    : frc::InstantCommand("Intake"),
      logger_(spdlog::get("command")),
      operation_(operation),
      is_started_(false) {
  Requires(Robot::intake);
}

void Intake::Initialize() {
  switch (operation_) {
    case kStart:
      Start();
      break;
    case kReverse:
      Reverse();
      break;
    case kStop:
      Stop();
      break;
    case kToggle:
      Toggle();
      break;
  }
}

void Intake::Start() {
  logger_->debug("starting intake");
  Robot::intake->Start();
  is_started_ = true;
}

void Intake::Reverse() {
  logger_->debug("reversing intake");
  // Robot::intake->Reverse();
  // is_started_ = true;
}

void Intake::Stop() {
  logger_->debug("stopping intake");
  Robot::intake->Stop();
  is_started_ = false;
}

void Intake::Toggle() {
  if (is_started_) {
    Stop();
    return;
  }
  Start();
}
