#include "sequence_01.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(78 * 50.72);
const int kStrafeDistance = static_cast<int>(49 * 50.72);
}

/**
 * Blue hopper dump and shoot
 */
Sequence01::Sequence01() : frc::CommandGroup("Sequence01") {
  AddSequential(new Log("starting BLUE alliance hopper dump and shoot"));

  // drive on slant to hopper
  AddSequential(new drive::TimedSwerveDrive(-0.50 / 2.0, -0.95 / 2.0, 2.0));

  AddSequential(new WaitCommand(2.0));

  // move out from wall
  AddSequential(new drive::TimedSwerveDrive(0.7, 0, 0.4));

  // azimuth
  AddSequential(new drive::TimedAzimuth(0.4, 0.8));
  //
  // // start shooting
  AddSequential(new StartShooting());
}
