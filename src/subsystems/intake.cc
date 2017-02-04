#include "intake.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

Intake::Intake(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Intake"), logger_(spdlog::get("subsystem")) {
  config->get_table("STEAMWORKS");
}
