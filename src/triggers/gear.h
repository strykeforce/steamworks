#pragma once

#include <WPILib.h>

namespace steamworks {
namespace trigger {

class Gear : public frc::Trigger {
 public:
  Gear();

  bool Get() override;

 private:
  frc::XboxController gamepad_;
};
} /* trigger */
} /* steamworks */
