#include "dashboard.h"

using namespace steamworks::command;

DashboardLight::DashboardLight(std::string label, bool state)
    : frc::InstantCommand("DashboardLightOn"), label_(label), state_(state) {}

void DashboardLight::Initialize() {
  SmartDashboard::PutBoolean(label_, state_);
}
