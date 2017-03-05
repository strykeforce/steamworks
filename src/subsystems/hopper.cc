#include "hopper.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/talon/settings.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

Hopper::Hopper(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Hopper"),
      logger_(spdlog::get("subsystem")),
      voltage_(7.0),
      is_running_(false) {
  auto steamworks_config = config->get_table("STEAMWORKS");

  auto voltage = steamworks_config->get_as<double>("hopper_voltage");
  if (voltage) {
    voltage_ = *voltage;
  } else {
    logger_->warn("STEAMWORKS hopper_voltage setting missing, using default");
  }
  logger_->info("hopper motor voltage: {}", voltage_);

  auto talon_settings = talon::Settings::Create(steamworks_config, "hopper");
  // logger_->debug("dumping hopper talon configuration");
  // talon_settings->LogConfig(logger_);
  talon_settings->Initialize(RobotMap::hopper_talon);
}

void Hopper::Start() {
  RobotMap::hopper_talon->Set(voltage_);
  is_running_ = true;
  SmartDashboard::PutNumber("Hopper Voltage", voltage_);
}

void Hopper::Stop() {
  RobotMap::hopper_talon->StopMotor();
  is_running_ = false;
}

bool Hopper::IsRunning() { return is_running_; }

double Hopper::GetVoltage() { return voltage_; }

void Hopper::SetVoltage(double volts) {
  voltage_ = volts;
  Start();
}
