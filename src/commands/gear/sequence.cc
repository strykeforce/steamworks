#include "sequence.h"

#include "commands/commands.h"

using namespace steamworks::command::gear;

StageGear::StageGear() : frc::CommandGroup("StageGear") {
  // TODO: check for gear before starting
  AddSequential(new DeployGear());
  AddSequential(new CloseClamp());
  AddSequential(new PivotGear());
}

TestGearLoad::TestGearLoad() : frc::CommandGroup("TestGearLoad") {
  AddSequential(new Log("loading gear"));
  AddSequential(new LoadGear());

  AddSequential(new Log("waiting to deploy gear"));
  AddSequential(new WaitCommand(2));

  AddSequential(new Log("deploying gear"));
  AddSequential(new DeployGear());

  AddSequential(new Log("clamping gear"));
  AddSequential(new CloseClamp());

  AddSequential(new Log("pivoting gear"));
  AddSequential(new PivotGear());
}
