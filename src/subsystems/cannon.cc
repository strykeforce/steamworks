#include "cannon.h"

#include <chrono>
#include <thread>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

using namespace avenger;

Cannon::Cannon(const std::shared_ptr<cpptoml::table> config)
    : ::Subsystem("Cannon"), logger_(spdlog::stdout_color_st(GetName())) {
  fire_duration_ms_ = config->get_as<int>("fire_duration_ms").value_or(0);

  for (size_t i = 0; i < 8; i++) {
    solenoids_.push_back(std::unique_ptr<::Solenoid>(new ::Solenoid(i)));
  }
}

void Cannon::Fire() {
  if (barrel_ == 8) {
    barrel_ = 0;
  }
  solenoids_[barrel_]->Set(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(fire_duration_ms_));
  solenoids_[barrel_]->Set(false);
  ++barrel_;
}
