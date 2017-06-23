#include "sequence_09.h"

#include "commands/deadeye/gear_led.h"
#include "commands/deadeye/mode.h"
#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/auton/place_gear.h"
#include "commands/gear/release.h"
#include "commands/gear/sequence.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"
#include "robot_map.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const double kTicksPerInch = 50.72;
const int kPrepareSpeed = 500;
const int kPrepareElevation = 5400;
}  // namespace

/**
 * BLUE right gear, shoot
 */
Sequence09::Sequence09() : frc::CommandGroup("Sequence09") {
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice
                        ? "BLUE alliance right gear, shoot on PRACTICE (09)"
                        : "BLUE alliance right gear, shoot on COMP (09)";
  AddSequential(new LogCommand(msg));

  // stage the gear
  AddParallel(new gear::StageGear());
  AddParallel(new deadeye::GearLED(true));

  // drive out
  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 400;
  dc.acceleration = 200;
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.segments.emplace_back(180, 96 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // rotate to face gear Lift
  AddSequential(new drive::GyroAzimuth(-60));

  // approach target
  AddSequential(new gear::PlaceGear(gear::Lift::right));

  // release gear
  AddParallel(new gear::ReleaseGear());
  AddParallel(new deadeye::GearLED(false));

  // spin up shooter
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  //  back off
  dc.max_speed = 400;
  dc.segments.clear();
  dc.segments.emplace_back(-60, 24 * kTicksPerInch);
  dc.segments.emplace_back(30, 24 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // pivot back towards boiler
  AddSequential(new drive::GyroAzimuth(70));

  // and shoot
  AddSequential(new StartShooting());
}
