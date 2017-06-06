#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {
class DriveZero : public frc::Command {
 public:
  DriveZero();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* drive */
} /* command */
} /* steamworks */
