#pragma once

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"
#include "serial/serial.h"

namespace deadeye {
class Link {
 public:
  enum Mode { kBoilerMode, kGearMode, kQuitMode };

  Link(std::shared_ptr<cpptoml::table> config);
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  Mode GetMode();
  void SendBoilerSolution(int azimuth_error, int range, int angle, int speed);
  void SendNoTarget();

 private:
  std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<serial::Serial> serial_;
};

} /* deadeye */
