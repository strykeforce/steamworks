#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {

class StartShooting : public frc::CommandGroup {
 public:
  StartShooting();
};

class StopShooting : public frc::CommandGroup {
 public:
  StopShooting();
};

} /* command */
} /* steamworks */
