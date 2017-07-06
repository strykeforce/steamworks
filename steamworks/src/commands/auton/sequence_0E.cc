#include "sequence_0E.h"

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
 * Blue right gear, run down field
 */
Sequence0E::Sequence0E() : frc::CommandGroup("Sequence0E") {
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice
                        ? "RED alliance left gear, shoot on PRACTICE (0E)"
                        : "RED alliance left gear, shoot on COMP (0E)";
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
  AddSequential(new drive::GyroAzimuth(60));

  // approach target
  AddSequential(new gear::PlaceGear(gear::Lift::left));

  // release gear
  AddParallel(new gear::ReleaseGear());
  AddParallel(new deadeye::GearLED(false));

  // spin up shooter
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  //  back off
  dc.max_speed = 400;
  dc.segments.clear();
  dc.segments.emplace_back(60, 28 * kTicksPerInch);
  dc.segments.emplace_back(-35, 18 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // pivot into drive alignment
  AddSequential(new drive::GyroAzimuth(-70));

  // and shoot
  AddSequential(new StartShooting());
}
