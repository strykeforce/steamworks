#include "sequence.h"

#include "commands/commands.h"

using namespace steamworks::command;

LoadGear::LoadGear() : frc::CommandGroup("LoadGear") {
  // AddSequential(new Log("ready loading gear"));
  // AddSequential(new gear::ReadyLoadGear());

  AddSequential(new Log("loading gear"));
  AddSequential(new gear::LoadGear());

  AddSequential(new Log("waiting to deploy gear"));
  AddSequential(new WaitCommand(2));

  AddSequential(new Log("deploying gear"));
  AddSequential(new gear::DeployGear());

  AddSequential(new Log("clamping gear"));
  AddSequential(new gear::CloseClamp());

  AddSequential(new Log("pivoting gear"));
  AddSequential(new gear::PivotGear());
}
