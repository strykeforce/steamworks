#include "sequence_03.h"

#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/log.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Command group for auton teting
 */
Sequence03::Sequence03() : frc::CommandGroup("Sequence03") {
  AddSequential(new LogCommand("starting center gear placement"));
  // AddParallel(new gear::StageGear());

  // AddSequential(new drive::TimedSwerveDrive(-0.15, 0, 4.6));

  //
  // AddSequential(new gear::ReleaseGear());
  //
}
