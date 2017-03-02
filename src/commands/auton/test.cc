#include "test.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(76 * 50.72);
const int kStrafeDistance = static_cast<int>(60 * 50.72);
const int kVelocity = 200;
}

/**
 * Command group for auton teting
 */
AutonTestSeq::AutonTestSeq() : frc::CommandGroup("AutonTestSeq") {
  AddSequential(new drive::PositionDrive(0));
  AddSequential(new drive::Drive(kForwardDistance));
}
