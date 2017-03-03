#include "sequence_03.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Command group for auton teting
 */
Sequence03::Sequence03() : frc::CommandGroup("Sequence03") {
  AddSequential(new Log("Sequence 03 starting"));
  // AddSequential(new gear::ZeroPivot());

  // AddSequential(new gear::StageGear());

  AddSequential(new Log("PositionDrive to Position 1 gear lift"));
  AddSequential(new drive::PositionDrive(kForwardAzimuth));
  AddSequential(new drive::Drive(74 * 50.72, kForwardAzimuth, 100));
  AddSequential(new WaitCommand(2));
  //
  // AddSequential(new gear::ReleaseGear());
  //
  AddSequential(new Log("PositionDrive away from Position 1 gear lift"));
  AddSequential(new drive::PositionDrive(0));
  AddSequential(new drive::Drive(25 * 50.72, 0, 100));
}
