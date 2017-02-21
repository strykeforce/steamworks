#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {
namespace gear {

class ReadyDeployGear : public frc::Command {
 public:
  ReadyDeployGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

class DeployGear : public frc::Command {
 public:
  DeployGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
};

} /* gear */
} /* command */
} /* steamworks */
