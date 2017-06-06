#pragma once

#include "WPILib.h"

namespace avenger {

class ResetCannon : public ::Command {
 private:
  /* data */

 public:
  ResetCannon();
  virtual ~ResetCannon() = default;

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};
} /* avenger */
