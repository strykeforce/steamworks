#pragma once

#include "WPILib.h"

#include "deploy.h"

namespace steamworks {
namespace command {
namespace gear {

const std::string kGearLoadedLabel = "Gear Loaded";

class ReadyLoadGear : public frc::Command {
 public:
  ReadyLoadGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

class LoadGear : public frc::Command {
 public:
  LoadGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* gear */
} /* command */
} /* steamworks */
