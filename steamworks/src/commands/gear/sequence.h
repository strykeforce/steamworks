#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace gear {
class StageGear : public frc::CommandGroup {
 public:
  StageGear();
};

class TestGearLoad : public frc::CommandGroup {
 public:
  TestGearLoad();
};

} /* gear */

} /* command */
} /* steamworks */
