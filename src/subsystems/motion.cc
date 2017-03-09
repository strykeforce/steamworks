#include "motion.h"

#include <WPILib.h>
#include <cmath>
#include <iostream>

#include "drive.h"

using namespace steamworks::subsystem;
using namespace std;

/**
 * Performs filter-based motion profiling.
 *
 * See posts 18 and 44 of this Chief Delphi [thread](https://goo.gl/pHN65k) for
 * an explanation of this method.
 *
 * @param distance in encoder ticks
 * @param max_v in ticks/100ms
 * @param max_v_time time to max velocity in milliseconds
 * @param max_a_time time to max acceleration in milliseconds
 * @param period in milliseconds
 */
Motion::Motion(SwerveDrive* drive, unsigned distance, unsigned max_v,
               unsigned max_v_time, unsigned max_a_time, unsigned period)
    : logger_(spdlog::get("subsystem")),
      drive_(drive),
      distance_(distance),
      t1_(max_v_time),
      t2_(max_a_time),
      v_prog_(max_v / 100.0),
      f1_len_(static_cast<unsigned>(ceil(t1_ / period))),
      f2_len_(static_cast<unsigned>(ceil(t2_ / period))),
      period_(period) {
  double t4 = distance_ / v_prog_;  // ms
  total_inputs_ = static_cast<unsigned>(ceil(t4 / period));

  notifier_ = std::make_unique<Notifier>(&Motion::Calculate, this);
  notifier_->StartPeriodic(period_ / 1000.0);
}

/**
 * Stop the notifier loop.
 */
Motion::~Motion() { notifier_->Stop(); }

/**
 * Perform an iteration of the motion profile.
 */
void Motion::Calculate() {
  // f1 is a boxcar filter
  int input = iteration_++ < total_inputs_ ? 1 : 0;

  // update the f1 filter
  f1_.push(input);
  f1_total_ += input;
  // remove old elements when filter bins are full
  if (f1_.size() > f1_len_) {
    f1_total_ -= f1_.front();
    f1_.pop();
  }
  double f1_output = f1_total_ / f1_len_;

  // update the f2 filter with the output of f1 filter
  f2_.push(f1_output);
  f2_total_ += f1_output;
  // remove old elements when filter bins are full
  if (f2_.size() > f2_len_) {
    f2_total_ -= f2_.front();
    f2_.pop();
  }

  // compute current velocity, acceleration and elapsed time
  curr_vel_ =
      (f1_total_ + f1_output + f2_total_) / (f1_len_ + f2_len_ + 1) * v_prog_;
  elapsed_distance_ += ((prev_vel_ + curr_vel_) / 2.0) * period_;
  accel_ = (curr_vel_ - prev_vel_) / period_;
  prev_vel_ = curr_vel_;
  elapsed_time_ += period_;

  SPDLOG_DEBUG(logger_, "motion driving at {}", curr_vel_ * 100);
  drive_->CrabDriveAutonomous(curr_vel_ * 100, 0);
}

/**
 * True when robot has finished travelling distance.
 */
bool Motion::OnTarget() {
  // FIXME: precalculate condition
  return iteration_ >= total_inputs_ + f1_len_ + f2_len_;
}
