#pragma once

#include <string>

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {
class Log : public frc::InstantCommand {
 public:
  Log(std::string message);

  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const std::string message_;
};
} /* command */
} /* steamworks */
