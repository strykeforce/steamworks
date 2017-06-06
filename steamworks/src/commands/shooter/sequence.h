#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {

class StartShooting : public frc::CommandGroup {
 public:
  StartShooting();

 protected:
  void Interrupted() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

/**
 * Manually targeted shot.
 */
class StartCloseShot : public frc::CommandGroup {
 public:
  StartCloseShot();

 protected:
  void Interrupted() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

/**
 * Stop or cancel shooting.
 */
class StopShooting : public frc::CommandGroup {
 public:
  StopShooting();
};

} /* command */
} /* steamworks */
