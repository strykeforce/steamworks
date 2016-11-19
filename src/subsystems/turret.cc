#include "turret.h"

#include <cassert>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"
#include "talon/settings.h"

using namespace avenger;
using namespace sidewinder;

Turret::Turret(const std::shared_ptr<cpptoml::table> config)
    : ::Subsystem("Turret"), logger_(spdlog::stdout_color_st(GetName())) {
  assert(config);
  logger_->set_level(spdlog::level::trace);
  logger_->trace("starting constructor");

  auto talon_settings = talon::Settings::Create(config, "turret");
  logger_->debug("dumping turret talon configuration");
  talon_settings->LogConfig(logger_);
}
