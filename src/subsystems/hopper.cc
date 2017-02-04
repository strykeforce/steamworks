#include "hopper.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

Hopper::Hopper(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Hopper"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
