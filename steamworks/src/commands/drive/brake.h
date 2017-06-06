#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {

class EnableBrake : public frc::Command {
 public:
  EnableBrake();
  virtual ~EnableBrake() = default;

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  /* data */
};

class DisableBrake : public frc::InstantCommand {
 public:
  DisableBrake();
  virtual ~DisableBrake() = default;

 private:
  /* data */
};
} /* drive */
} /* command */
} /* steamworks */
