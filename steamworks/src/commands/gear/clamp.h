#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace gear {

class CloseClamp : public frc::TimedCommand {
 public:
  CloseClamp();

 protected:
  void Initialize() override;
};

} /* gear */
} /* command */
} /* steamworks */
