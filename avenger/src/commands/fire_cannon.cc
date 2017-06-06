#include "fire_cannon.h"

#include <cassert>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "robot.h"

using namespace avenger;

FireCannon::FireCannon() : ::Command("FireCannon") {
  assert(Robot::cannon);
  Requires(Robot::cannon);
}

void FireCannon::Initialize() { Robot::cannon->Fire(); }

void FireCannon::Execute() {}

bool FireCannon::IsFinished() { return true; }

void FireCannon::End() {}

void FireCannon::Interrupted() {}
