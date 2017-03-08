#include "sequence.h"

#include "commands/commands.h"
#include "robot.h"

using namespace steamworks::command;

namespace {
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;
}

StartShooting::StartShooting()
    : frc::CommandGroup("StartShooting"), logger_(spdlog::get("command")) {
  // Requires(Robot::shooter_elevation);
  // Requires(Robot::shooter_wheel);
  SetInterruptible(true);
  AddSequential(new Log("start shooting setup"));
  AddParallel(new deadeye::ShooterLED(true));
  AddSequential(new shooter::StartupShot(kPrepareSpeed, kPrepareElevation));

  AddParallel(new drive::DeadeyeAzimuth());
  AddSequential(new shooter::GetAngle());

  AddParallel(new deadeye::ShooterLED(false));
  AddParallel(new shooter::SetWheel());
  AddSequential(new shooter::SetElevation());

  AddSequential(new StartHopper());
}

/**
 * Called if shooting sequence cancelled by button or bad solution.
 */
void StartShooting::Interrupted() {
  logger_->warn("StartShooting shooting sequence cancelled");
  End();
}

StopShooting::StopShooting() : frc::CommandGroup("StopShooting") {
  // Requires(Robot::shooter_elevation);
  // Requires(Robot::shooter_wheel);
  // SetInterruptible(true);
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
