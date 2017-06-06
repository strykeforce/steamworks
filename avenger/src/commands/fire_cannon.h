#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace avenger {

class FireCannon : public ::Command {
 private:
  /* data */

 public:
  FireCannon();
  virtual ~FireCannon() = default;

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};
} /* avenger */
