#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace gear {
class StartClear : public frc::InstantCommand {
 public:
  StartClear();
  virtual ~StartClear() = default;

 protected:
  void Initialize() override;
};

class StopClear : public frc::InstantCommand {
 public:
  StopClear();
  virtual ~StopClear() = default;

 protected:
  void Initialize() override;
};

} /* gear */
} /* command */
} /* steamworks */
