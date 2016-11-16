#include "robot.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "robot_map.h"
#include "subsystems/drive.h"
#include "swerve/talon_map.h"
#include "talon/position_talon.h"

namespace sidewinder {

OI* Robot::oi = nullptr;
Drive* Robot::drive = nullptr;

Robot::Robot() : IterativeRobot(), logger_(spdlog::stdout_color_st("robot")) {
  logger_->set_level(spdlog::level::trace);
}

void Robot::RobotInit() {
  LoadConfig();
  RobotMap::Init(config_);
  LogAbsoluteEncoders();

  // load in order of dependency
  drive = new Drive(config_->get_table("SIDEWINDER"));
  oi = new OI(config_);
}

void Robot::DisabledInit() { logger_->trace("DisabledInit"); }

void Robot::DisabledPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() { logger_->trace("AutonomousInit"); }

void Robot::AutonomousPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() { logger_->trace("TeleopInit"); }

void Robot::TeleopPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { logger_->trace("TestInit"); }

void Robot::TestPeriodic() { ::LiveWindow::GetInstance()->Run(); }

void Robot::LogAbsoluteEncoders() {
  auto tm = RobotMap::swerve_talons;
  auto pos = tm->lf_azimuth->GetPulseWidthPosition() & 0xFFF;
  logger_->info("left front azimuth position: {}", pos);

  pos = tm->rf_azimuth->GetPulseWidthPosition() & 0xFFF;
  logger_->info("right front azimuth position: {}", pos);

  pos = tm->lr_azimuth->GetPulseWidthPosition() & 0xFFF;
  logger_->info("left rear azimuth position: {}", pos);

  pos = tm->rr_azimuth->GetPulseWidthPosition() & 0xFFF;
  logger_->info("right rear azimuth position: {}", pos);
}

void Robot::LoadConfig() {
  logger_->info("loading configuration from: /home/lvuser/drivetest.toml");
  config_ = cpptoml::parse_file("/home/lvuser/drivetest.toml");
  assert(config_);
}

} /* sidewinder */

START_ROBOT_CLASS(sidewinder::Robot)
