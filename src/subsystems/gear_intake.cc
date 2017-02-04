#include "gear_intake.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

GearIntake::GearIntake(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("GearIntake"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
