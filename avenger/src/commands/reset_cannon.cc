#include "reset_cannon.h"

#include <cassert>

#include "WPILib.h"

#include "robot.h"

using namespace avenger;

ResetCannon::ResetCannon() : ::Command("ResetCannon") {
  assert(Robot::cannon);
  Requires(Robot::cannon);
}

void ResetCannon::Initialize() { Robot::cannon->Reset(); }

void ResetCannon::Execute() {}

bool ResetCannon::IsFinished() { return true; }

void ResetCannon::End() {}

void ResetCannon::Interrupted() {}
