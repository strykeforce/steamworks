#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {

class DriveTeleOp : public frc::Command {
 public:
  DriveTeleOp();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
};

} /* drive */
} /* command */
} /* steamworks */
