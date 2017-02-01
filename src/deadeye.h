#pragma once

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "link.h"
#include "camera.h"

namespace deadeye {

class Deadeye {
 public:
  Deadeye(std::shared_ptr<cpptoml::table> config);
  Deadeye(const Deadeye&) = delete;
  Deadeye& operator=(const Deadeye&) = delete;

  void Run();  // start the main loop

 private:
  std::shared_ptr<spdlog::logger> logger_;
  Link link_;
  Camera boiler_camera_;

  void ProcessBoilerTarget();
  void ProcessGearTarget();
};

} /* deadeye */
