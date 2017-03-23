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
  double azimuth_error_;
  double azimuth_abs_error_;
  double height_;
  double height_error_;
  double height_abs_error_;
  int stable_count_;
  // std::unique_ptr<motion::AzimuthMotion> azimuth_controller_;
  // std::unique_ptr<motion::AzimuthMotion> distance_controller_;

  double CalculateAzimuthSetpoint();
  double CalculateDriveSetpoint();
};

} /* gear */
} /* command */
} /* steamworks */
