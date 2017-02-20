#include "gear.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

GearLoader::GearLoader(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("GearLoader"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
