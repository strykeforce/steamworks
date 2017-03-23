#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace motion {
class AzimuthMotion;
} /* motion */

namespace command {
namespace gear {

class PlaceGear : public frc::Command {
 public:
  PlaceGear();
  virtual ~PlaceGear() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double strafe_error_;
  double strafe_abs_error_;
  int stable_count_;
  // std::unique_ptr<motion::AzimuthMotion> azimuth_controller_;
  // std::unique_ptr<motion::AzimuthMotion> distance_controller_;
};

} /* gear */
} /* command */
} /* steamworks */
