#pragma once

#include "WPILib.h"

namespace steamworks {
namespace command {

class SetRange : public frc::Command {
 public:
  SetRange();

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  int elevation_;
  int speed_;
};
} /* command */
} /* steamworks */
