#pragma once

#include <WPILib.h>

namespace steamworks {
namespace trigger {

class GearTrigger : public frc::Trigger {
 public:
  GearTrigger();

  bool Get() override;

 private:
  frc::XboxController gamepad_;
};
} /* trigger */
} /* steamworks */
