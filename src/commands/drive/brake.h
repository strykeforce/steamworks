#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {

class EnableBrake : public frc::InstantCommand {
 public:
  EnableBrake();
  virtual ~EnableBrake() = default;

 protected:
  void Initialize() override;

 private:
  /* data */
};

} /* drive */
} /* command */
} /* steamworks */
