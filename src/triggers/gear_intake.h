#pragma once

#include <WPILib.h>

namespace steamworks {
namespace trigger {

class GearIntake : public frc::Trigger {
 public:
  GearIntake();

  bool Get() override;
};
} /* trigger */
} /* steamworks */
