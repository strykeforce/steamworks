#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

#include "link.h"
#include "camera.h"
#include "link/mode.h"

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
  Mode current_mode_ = Mode::quit;

  void SwitchMode(Mode mode);
  void ProcessBoilerTarget();
  void ProcessGearTarget();
};

} /* deadeye */
