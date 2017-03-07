#include "sequence.h"

#include "commands/commands.h"
#include "robot.h"

using namespace steamworks::command;

namespace {
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;
}

StartShooting::StartShooting() : frc::CommandGroup("StartShooting") {
  Requires(Robot::shooter);
  SetInterruptible(true);
  AddSequential(new Log("start shooting setup"));
  AddParallel(new deadeye::ShooterLED(true));
  AddParallel(new shooter::StartupShot(kPrepareSpeed, kPrepareElevation));

  AddSequential(new Log("start azimuth"));
  AddSequential(new drive::DeadeyeAzimuth());

  AddSequential(new Log(
      "start elevation angle measurement and calculating shooter solution"));
  AddSequential(new shooter::GetAngle());

  AddParallel(new deadeye::ShooterLED(false));
  AddSequential(new Log("setting solution elevation and speed"));

  AddParallel(new shooter::SetWheel());
  AddSequential(new shooter::SetElevation());

  AddSequential(new StartHopper());
}

StopShooting::StopShooting() : frc::CommandGroup("StopShooting") {
  Requires(Robot::shooter);
  SetInterruptible(false);
  AddParallel(new deadeye::ShooterLED(false));
  AddParallel(new StopHopper());
  AddSequential(new StopShooterWheel());
}

TimedTest::TimedTest(std::string name, double timeout)
    : frc::TimedCommand("TimedTest", timeout),
      logger_(spdlog::get("command")),
      name_(name) {}

void TimedTest::Initialize() {
  logger_->info("TimedTest {} initialized", name_);
}

void TimedTest::End() {
  logger_->info("TimedTest {} ended, time since initialization {}", name_,
                TimeSinceInitialized());
}
