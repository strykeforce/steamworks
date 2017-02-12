#pragma once

#include <string>

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {
class Intake : public frc::InstantCommand {
 public:
  enum Operation {
    kStart,
    kReverse,
    kStop,
    kToggle,
  };

  Intake(Operation operation);

  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  Operation operation_;
  bool is_started_;

  void Start();
  void Reverse();
  void Stop();
  void Toggle();
};
} /* command */
} /* steamworks */
