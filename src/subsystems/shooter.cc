#include "shooter.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

Shooter::Shooter(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Shooter"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
