#include "sequence_04.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Blue move out and shoot 10
 */
Sequence04::Sequence04() : frc::CommandGroup("Sequence04") {
  AddSequential(new Log("starting BLUE alliance move out and and shoot"));
  AddSequential(new drive::TimedSwerveDrive(-0.25, 0, 2.2));

  // azimuth
  AddSequential(new drive::TimedAzimuth(0.47, 0.3));

  // // start shooting
  AddSequential(new StartShooting());
}
