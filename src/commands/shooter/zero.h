#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {
namespace shooter {

/**
 * PositionElevationZero moves the elevation to the limit switch in preparation
 * for zeroing.
 */
class PositionElevationZero : public frc::Command {
 public:
  PositionElevationZero();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

/**
 * ZeroElevation pushes against the zero stop until stable successive encoder
 * readings.
 */
class ZeroElevation : public frc::Command {
 public:
  ZeroElevation();

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  unsigned last_encoder_ = 0;
  int stable_count_ = 0;
};

/**
 * SetElevationZero sets the elevation enoder to zero and returns elevation to
 * normal operation.
 */
class SetElevationZero : public frc::InstantCommand {
 public:
  SetElevationZero();

 protected:
  void Initialize() override;
  void End() override;
};

} /* shooter */

class SetElevationZero : public frc::CommandGroup {
 public:
  SetElevationZero();
};

} /* command */
} /* steamworks */
