#include "sequence.h"

#include "clamp.h"
#include "commands/log.h"
#include "deploy.h"
#include "load.h"
#include "pivot.h"

using namespace steamworks::command::gear;

StageGear::StageGear() : frc::CommandGroup("StageGear") {
  // TODO: check for gear before starting
  AddSequential(new DeployGear());
  AddSequential(new CloseClamp());
  AddSequential(new PivotGear());
}

TestGearLoad::TestGearLoad() : frc::CommandGroup("TestGearLoad") {
  AddSequential(new LogCommand("loading gear"));
  AddSequential(new LoadGear());

  AddSequential(new LogCommand("waiting to deploy gear"));
  AddSequential(new WaitCommand(2));

  AddSequential(new LogCommand("deploying gear"));
  AddSequential(new DeployGear());

  AddSequential(new LogCommand("clamping gear"));
  AddSequential(new CloseClamp());

  AddSequential(new LogCommand("pivoting gear"));
  AddSequential(new PivotGear());
}
