#include "sequence_01.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(78 * 50.72);
const int kStrafeDistance = static_cast<int>(49 * 50.72);
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;

double inches(double x) { return 50.72 * x; }
}

/**
 * Blue hopper dump and shoot
 */
Sequence01::Sequence01() : frc::CommandGroup("Sequence01") {
  AddSequential(new LogCommand("starting BLUE alliance hopper dump and shoot"));

  // drive out
  // AddSequential(new drive::Drive(-90, inches(76)));
  AddSequential(new drive::Drive(-115, inches(86)));

  // drive into hopper
  AddSequential(new drive::Drive(180, inches(36), 0.5));

  // wait for balls
  AddSequential(new WaitCommand(1.5));

  // drive out from hopper
  AddSequential(new drive::Drive(0, inches(20)));

  // pre-azimuth towards boiler
  AddSequential(new drive::GyroAzimuth(125));

  // // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddSequential(new StartIntake());
}
