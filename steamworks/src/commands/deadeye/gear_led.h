#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace deadeye {

class GearLED : public frc::InstantCommand {
 public:
  GearLED(bool enabled);
  virtual ~GearLED() = default;

 protected:
  void Initialize() override;

 private:
  bool enabled_;
};

} /* deadeye */
} /* command */
} /* deadeye */
