#include "camera.h"

#include "cpptoml/cpptoml.h"

using namespace steamworks::subsystem;

Camera::Camera(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Camera"), logger_(spdlog::get("subsystem")) {
  auto steamworks_config = config->get_table("STEAMWORKS");
}
