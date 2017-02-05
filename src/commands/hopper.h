#pragma once

#include <string>

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {
class Hopper : public frc::InstantCommand {
 public:
  enum Operation {
    kStart,
    kStop,
    kToggle,
  };

  Hopper(Operation operation);

  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  Operation operation_;
  bool is_started_;

  void Start();
  void Stop();
  void Toggle();
};
} /* command */
} /* steamworks */
