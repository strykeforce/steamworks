#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {
class Zero : public frc::Command {
 public:
  Zero();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* drive */
} /* command */
} /* steamworks */
