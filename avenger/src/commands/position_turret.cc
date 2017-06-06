#include "position_turret.h"

#include <cassert>

#include "WPILib.h"

#include "robot.h"

using namespace avenger;

PositionTurret::PositionTurret() : ::Command("PositionTurret") {
  assert(Robot::turret);
  Requires(Robot::turret);
}

void PositionTurret::Initialize() {}

void PositionTurret::Execute() {
  float elevation = Robot::oi->GetTurretElevationAxis();
  Robot::turret->Move(elevation);
}

bool PositionTurret::IsFinished() { return false; }

void PositionTurret::End() { Robot::turret->Move(0.0); }

void PositionTurret::Interrupted() { End(); }
