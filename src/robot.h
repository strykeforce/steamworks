#pragma once

#include <memory>
#include <string>

#include "cpptoml/cpptoml.h"
#include "serial/serial.h"

namespace deadeye {

class Robot {
 private:
  std::unique_ptr<serial::Serial> serial_{nullptr};

 public:
  Robot(std::shared_ptr<cpptoml::table> config);
  virtual ~Robot() = default;

  void SendBoilerTarget(int azimuth_error, int range);
  void SendBoilerNoTarget();
};

} /* deadeye */
