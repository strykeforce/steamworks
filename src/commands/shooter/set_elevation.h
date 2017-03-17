#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace shooter {

class SetElevation : public frc::Command {
 public:
  SetElevation();

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  double elevation_;
};

} /* shooter */
} /* command */
} /* steamworks */
