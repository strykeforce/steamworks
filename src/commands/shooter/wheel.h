#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {

class StartShooterWheel : public frc::InstantCommand {
 public:
  StartShooterWheel();

 protected:
  void Initialize() override;
};

class StopShooterWheel : public frc::Command {
 public:
  StopShooterWheel();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;

 private:
  unsigned speed_setpoint_;
};

class IncrementShooterSpeed : public frc::InstantCommand {
 public:
  IncrementShooterSpeed();

 protected:
  void Initialize() override;
};

class DecrementShooterSpeed : public frc::InstantCommand {
 public:
  DecrementShooterSpeed();

 protected:
  void Initialize() override;
};
} /* command */
} /* steamworks */
