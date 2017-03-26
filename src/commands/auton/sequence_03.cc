#include "sequence_03.h"

#include "commands/deadeye/gear_led.h"
#include "commands/deadeye/mode.h"
#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/auton/place_gear.h"
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
  AddSequential(new LogCommand("starting TEST sequence"));
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));
  AddSequential(new deadeye::GearLED(true));
  AddSequential(new gear::PlaceGear(gear::Lift::center));
}
