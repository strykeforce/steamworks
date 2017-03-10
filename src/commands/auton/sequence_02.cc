#include "sequence_02.h"

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
 * Red hopper dump and shoot
 */
Sequence02::Sequence02() : frc::CommandGroup("Sequence02") {
  AddSequential(new Log("starting RED alliance hopper dump and shoot"));

  // drive on slant to hopper
  AddSequential(new drive::TimedSwerveDrive(-0.50 / 2.0, 0.95 / 2.0, 1.75));

  AddSequential(new WaitCommand(2.0));

  // move out from wall
  AddSequential(new drive::TimedSwerveDrive(0.7, 0, 0.6));

  // azimuth
  AddSequential(new drive::TimedAzimuth(-0.4, 0.6));
  //
  // // start shooting
  // AddSequential(new StartShooting());
}
