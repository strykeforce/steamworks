#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

#include "link.h"
#include "boiler/boiler_camera.h"
#include "gear/gear_camera.h"

#include "config.h"
#ifdef LOG_FPS
#include "fps.h"
#endif

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
  BoilerCamera boiler_camera_;
  GearCamera gear_camera_;
  int current_mode_ = Link::Mode::idle;

#ifdef LOG_FPS
  int display_framerate_int_ = 0;
  FPS fps_;
#endif

#ifdef LOG_BOILER
  int display_boiler_int_ = 0;
#endif

#ifdef LOG_GEAR
  int display_gear_int_ = 0;
#endif

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void SwitchMode(int mode);
  void ProcessBoilerTarget();
  void ProcessGearTarget();
};

} /* deadeye */
