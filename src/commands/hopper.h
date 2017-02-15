#pragma once

#include <string>

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {
class StartHopper : public frc::InstantCommand {
 public:
  StartHopper();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class StopHopper : public frc::InstantCommand {
 public:
  StopHopper();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class ToggleHopper : public frc::InstantCommand {
 public:
  ToggleHopper();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  StartHopper start_cmd_;
  StopHopper stop_cmd_;
};

} /* command */
} /* steamworks */
