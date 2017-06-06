#pragma once

#include <WPILib.h>

namespace steamworks {
namespace trigger {

class Trim : public frc::Trigger {
 public:
  enum Direction {
    kUp,
    kDown,
    kLeft,
    kRight,
  };

  Trim(Direction direction);

  bool Get() override;

 private:
  int angle_;
  frc::XboxController gamepad_;
};

} /* trigger */
} /* steamworks */
