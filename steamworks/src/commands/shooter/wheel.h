#pragma once

#include <WPILib.h>

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
  IncrementShooterSpeed(double amount);

 protected:
  void Initialize() override;

 private:
  double amount_;
};

class DecrementShooterSpeed : public frc::InstantCommand {
 public:
  DecrementShooterSpeed(double amount);

 protected:
  void Initialize() override;

 private:
  double amount_;
};
} /* command */
} /* steamworks */
