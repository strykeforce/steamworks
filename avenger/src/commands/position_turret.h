#pragma once

#include "WPILib.h"

namespace avenger {

class PositionTurret : public ::Command {
 private:
  /* data */

 public:
  PositionTurret();
  virtual ~PositionTurret() = default;

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};
} /* avenger */
