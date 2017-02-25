#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <queue>

namespace frc {
class Notifier;
} /* frc */

namespace steamworks {
namespace subsystem {

class SwerveDrive;

class Motion {
 public:
  Motion(SwerveDrive* drive, unsigned distance, unsigned max_v, unsigned time_v,
         unsigned time_a, unsigned period);
  virtual ~Motion();

  void Calculate();
  bool OnTarget();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  SwerveDrive* drive_;
  std::unique_ptr<frc::Notifier> notifier_;
  double distance_;
  unsigned t1_;
  unsigned t2_;
  double v_prog_;
  unsigned f1_len_;
  unsigned f2_len_;
  unsigned total_inputs_;
  unsigned period_;
  unsigned iteration_ = 0;

  std::queue<int> f1_;
  double f1_total_ = 0;
  std::queue<double> f2_;
  double f2_total_ = 0;

  double elapsed_distance_ = 0;
  double elapsed_time_ = 0;
  double curr_vel_ = 0;
  double prev_vel_ = 0;
  double accel_ = 0;

  unsigned count_;
};

} /* subsystem */
} /* steamworks */
