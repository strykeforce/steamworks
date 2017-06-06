#include "sequence_05.h"

#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"
#include "robot_map.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Red move out and shoot 10
 */
Sequence05::Sequence05() : frc::CommandGroup("Sequence05") {
  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "RED alliance out and shoot on PRACTICE (05)"
                                : "RED alliance out and shoot on COMP (05)";
  AddSequential(new LogCommand(msg));

  // AddSequential(new drive::TimedSwerveDrive(-0.25, 0, 2.2));

  // azimuth
  // AddSequential(new drive::TimedAzimuth(-0.4, 0.3));

  // // start shooting
  // AddSequential(new StartShooting());
}
