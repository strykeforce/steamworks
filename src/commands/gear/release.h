#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {
namespace gear {
class ReleaseGear : public frc::Command {
 public:
  ReleaseGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};
} /* gear */
} /* command */
} /* steamworks */
