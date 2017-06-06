#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace deadeye {

class ShooterLED : public frc::InstantCommand {
 public:
  ShooterLED(bool enabled);
  virtual ~ShooterLED() = default;

 protected:
  void Initialize() override;

 private:
  bool enabled_;
};

} /* deadeye */
} /* command */
} /* deadeye */
