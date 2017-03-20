#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>
// #include <fstream>

namespace steamworks {
namespace command {
namespace drive {

struct DriveSegment {
  double angle;
  double distance;
  DriveSegment(double angle, double distance)
      : angle(angle), distance(distance) {}
};

struct DriveConfig {
  // double angle;
  // double distance;
  double min_speed;
  double max_speed;
  double acceleration;
  double deacceleration;
  double close_enough;
  double timeout = -1.0;
  std::vector<DriveSegment> segments;
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

  // const double angle_;  // robot relative -180 to 180
  std::vector<DriveSegment> segments_;
  std::vector<DriveSegment>::iterator segments_it_;
  double distance_;
  double segment_end_distance_ = 0;
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

  int stable_count_ = 0;
  int zero_count_ = 0;

  void StartNextSegment();
};
} /* drive */
} /* command */
} /* steamworks */
