#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace gear {

class PivotGear : public frc::Command {
 public:
  PivotGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

} /* gear */
} /* command */
} /* steamworks */
