#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {
namespace shooter {

class SetWheel : public frc::Command {
 public:
  SetWheel();

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  double speed_;
};

} /* shooter */
} /* command */
} /* steamworks */
