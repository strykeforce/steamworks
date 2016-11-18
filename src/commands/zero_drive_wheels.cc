#include "zero_drive_wheels.h"

#include <cassert>
#include "WPILib.h"

#include "robot.h"
#include "subsystems/drive.h"

using namespace avenger;

ZeroDriveWheels::ZeroDriveWheels() : ::Command("ZeroDriveWheels") {
  assert(Robot::drive);
  Requires(Robot::drive);
}

void ZeroDriveWheels::Initialize() { Robot::drive->ZeroWheels(); }

void ZeroDriveWheels::Execute() {}

bool ZeroDriveWheels::IsFinished() { return true; }

void ZeroDriveWheels::End() {}

void ZeroDriveWheels::Interrupted() {}
