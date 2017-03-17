#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {

class SetShooterElevation : public frc::Command {
 public:
  SetShooterElevation(unsigned elevation);

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  unsigned elevation_;
};

class IncrementShooterElevation : public frc::InstantCommand {
 public:
  IncrementShooterElevation();

 protected:
  void Initialize() override;
};

class DecrementShooterElevation : public frc::InstantCommand {
 public:
  DecrementShooterElevation();

 protected:
  void Initialize() override;
};
} /* command */
} /* steamworks */
