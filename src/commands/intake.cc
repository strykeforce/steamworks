#include "intake.h"

#include "robot.h"
#include "subsystems/intake.h"

using namespace steamworks::command;

namespace {
constexpr int kRunningSpeed = 6800;
}

//
// StartIntake
//
StartIntake::StartIntake()
    : frc::Command("StartIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void StartIntake::Initialize() {
  logger_->trace(__PRETTY_FUNCTION__);
  if (Robot::intake->GetEncoderVelocity() < 0) {
    Robot::intake->Stop();
  }
}

void StartIntake::Execute() {
  logger_->trace(__PRETTY_FUNCTION__);
  if (Robot::intake->GetEncoderVelocity() >= 0) {
    Robot::intake->Start();
  }
}

bool StartIntake::IsFinished() {
  logger_->trace(__PRETTY_FUNCTION__);
  logger_->debug("encoder velocity = {}", Robot::intake->GetEncoderVelocity());
  return Robot::intake->GetEncoderVelocity() > kRunningSpeed;
}

//
// StopIntake
//
StopIntake::StopIntake()
    : frc::Command("StopIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void StopIntake::Initialize() {
  logger_->trace(__PRETTY_FUNCTION__);
  Robot::intake->Stop();
}

bool StopIntake::IsFinished() {
  logger_->trace(__PRETTY_FUNCTION__);
  logger_->debug("encoder velocity = {}", Robot::intake->GetEncoderVelocity());
  return Robot::intake->GetEncoderVelocity() == 0;
}

//
// ClearIntake
//
ClearIntake::ClearIntake()
    : frc::Command("ClearIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void ClearIntake::Initialize() {
  logger_->trace(__PRETTY_FUNCTION__);
  if (Robot::intake->GetEncoderVelocity() > 0) {
    Robot::intake->Stop();
  }
}

void ClearIntake::Execute() {
  logger_->trace(__PRETTY_FUNCTION__);
  if (Robot::intake->GetEncoderVelocity() <= 0) {
    Robot::intake->Reverse();
  }
}

bool ClearIntake::IsFinished() {
  logger_->trace(__PRETTY_FUNCTION__);
  logger_->debug("encoder velocity = {}", Robot::intake->GetEncoderVelocity());
  return Robot::intake->GetEncoderVelocity() < -kRunningSpeed;
}
