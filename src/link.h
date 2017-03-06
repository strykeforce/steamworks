#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>
#include <serial/serial.h>

#include "link/mode.h"

namespace deadeye {
class Link {
 public:
  Link(std::shared_ptr<cpptoml::table> config);
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  Mode GetMode();
  void SendBoilerSolution(int azimuth_error, int centerline_error);
  void SendGearSolution(int azimuth_error, int range);
  void SendNoTarget();

 private:
  std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<serial::Serial> serial_;
};

} /* deadeye */
