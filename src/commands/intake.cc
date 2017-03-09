#include "intake.h"

#include "robot.h"
#include "subsystems/intake.h"

using namespace steamworks::command;

namespace {
constexpr int kStoppedSpeed = 10;
constexpr int kRunningSpeed = 6600;
}

//
// StartIntake
//
StartIntake::StartIntake()
    : frc::Command("StartIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void StartIntake::Initialize() {
  if (Robot::intake->GetSpeed() < 0) {
    Robot::intake->Stop();
  }
}

void StartIntake::Execute() {
  if (Robot::intake->GetSpeed() >= 0) {
    Robot::intake->Start();
  }
}

bool StartIntake::IsFinished() {
  return Robot::intake->GetSpeed() > kRunningSpeed;
}

//
// StopIntake
//
StopIntake::StopIntake()
    : frc::Command("StopIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void StopIntake::Initialize() { Robot::intake->Stop(); }

bool StopIntake::IsFinished() {
  return std::abs(Robot::intake->GetSpeed()) < kStoppedSpeed;
}

//
// ClearIntake
//
ClearIntake::ClearIntake()
    : frc::Command("ClearIntake"), logger_(spdlog::get("command")) {
  Requires(Robot::intake);
}

void ClearIntake::Initialize() {
  if (Robot::intake->GetSpeed() > 0) {
    Robot::intake->Stop();
  }
}

void ClearIntake::Execute() {
  if (Robot::intake->GetSpeed() <= 0) {
    Robot::intake->Reverse();
  }
}

bool ClearIntake::IsFinished() {
  return Robot::intake->GetSpeed() < -kRunningSpeed;
}
