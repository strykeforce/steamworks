#include "robot.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "robot_map.h"
#include "talon/voltage_talon.h"

namespace sidewinder {

Robot::Robot() : logger_(spdlog::stdout_color_st("Benchtest")) {
  logger_->set_level(spdlog::level::debug);
}

void Robot::RobotInit() {
  LoadConfig();
  RobotMap::Init();
  talon::Talon* vt = new talon::VoltageTalon(
      config_->get_table("SIDEWINDER")->get_table("AZIMUTH"));
  vt->Configure(RobotMap::lf_drive_talon);
  vt->SetMode(RobotMap::lf_drive_talon);
  vt->DumpToLog(logger_);
}

void Robot::DisabledInit() {
  logger_->trace("DisabledInit");
  RobotMap::lf_drive_talon->Set(0.0);
}

void Robot::DisabledPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() { logger_->trace("AutonomousInit"); }

void Robot::AutonomousPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  logger_->trace("TeleopInit");
  RobotMap::lf_drive_talon->Set(2.0);
}

void Robot::TeleopPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { logger_->trace("TestInit"); }

void Robot::TestPeriodic() { ::LiveWindow::GetInstance()->Run(); }

/** Reads our configuration file from ~lvuser/sidewinder.toml.
 * If not present or unable to parse, will read the default compiled-in
 * configuration from conf/default_config.toml.
 */
void Robot::LoadConfig() {
  logger_->info("loading configuration from: /home/lvuser/benchtest.toml");
  config_ = cpptoml::parse_file("/home/lvuser/benchtest.toml");
}

} /* sidewinder */

START_ROBOT_CLASS(sidewinder::Robot)
