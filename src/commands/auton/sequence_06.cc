#include "sequence_06.h"

#include "commands/deadeye/gear_led.h"
#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/auton/place_gear.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const double kTicksPerInch = 50.72;
const int kPrepareSpeed = 440;
const int kPrepareElevation = 1300;
}

/**
 * Red move out and shoot 10
 */
Sequence06::Sequence06() : frc::CommandGroup("Sequence06") {
  AddSequential(new LogCommand("starting BLUE alliance center gear"));

  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 100;
  dc.acceleration = 200;
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.segments.emplace_back(-180, 60 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  AddSequential(new deadeye::GearLED(true));
  // AddSequential(new gear::PlaceGear());
}
