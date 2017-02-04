#include "climber.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

Climber::Climber(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Climber"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
