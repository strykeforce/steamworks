#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {
namespace gear {

class ZeroPivot : public frc::Command {
 public:
  ZeroPivot();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  int stable_count_ = 0;
  int last_encoder_ = 0;
};
} /* gear */
} /* command */
} /* steamworks */
