#include "mode.h"

#include "robot.h"
#include "subsystems/deadeye.h"

using namespace steamworks::command::deadeye;

//
// EnableBoilerCamera
//
EnableCamera::EnableCamera(Mode mode)
    : frc::InstantCommand("EnableCamera"), mode_(mode) {}

void EnableCamera::Initialize() {
  switch (mode_) {
    case Mode::boiler:
      Robot::deadeye->EnableBoilerCamera();
      break;
    case Mode::gear:
      Robot::deadeye->EnableGearCamera();
      break;
  }
}

// void EnableGearCamera::Initialize() { Robot::deadeye->EnableGearCamera(); }
