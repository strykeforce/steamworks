#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>
// #include <fstream>

namespace steamworks {
namespace command {
namespace drive {

struct DriveConfig {
  double angle;
  double distance;
  double min_speed;
  double max_speed;
  double acceleration;
  double deacceleration;
  double close_enough;
  double timeout = -1.0;
};

class Drive : public frc::Command {
 public:
  // this is called by other constructors
  Drive(const DriveConfig& config);
  virtual ~Drive() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  // std::unique_ptr<std::ofstream> data_;

  const double angle_;  // robot relative -180 to 180
  const double distance_;
  const double min_speed_;
  const double max_speed_;
  const double close_enough_;

  double dead_zone_;
  double accel_dist_;
  double deaccel_dist_;
  double accel_done_pos_;

  double forward_factor_;
  double strafe_factor_;

  double error_;
  double abs_error_;
  double start_decel_pos_;

  int stable_count_ = 0;
  int zero_count_ = 0;
};
} /* drive */
} /* command */
} /* steamworks */
