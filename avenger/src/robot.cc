#include "robot.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "default_config.h"
#include "robot_map.h"
#include "sidewinder/swerve/talon_map.h"
#include "subsystems/cannon.h"
#include "subsystems/drive.h"
#include "subsystems/turret.h"
#include "version.h"

using namespace avenger;

OI* Robot::oi = nullptr;
AvengerDrive* Robot::drive = nullptr;
Turret* Robot::turret = nullptr;
Cannon* Robot::cannon = nullptr;

Robot::Robot() : IterativeRobot(), logger_(spdlog::stdout_color_st("robot")) {
  logger_->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("sidewinder")->set_level(spdlog::level::info);
}

void Robot::RobotInit() {
  LogVersion();
  LoadConfig();
  RobotMap::Init(config_);
  LogAbsoluteEncoders();

  // load in order of dependency
  drive = new AvengerDrive(config_);
  logger_->debug("done initializing AvengerDrive");
  turret = new Turret(config_->get_table("AVENGER"));
  logger_->debug("done initializing Turret");
  cannon = new Cannon(config_->get_table("AVENGER"));
  logger_->debug("done initializing Cannon");
  oi = new OI(config_);
  logger_->debug("done initializing OI");
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

/** Reads our configuration file from ~lvuser/stronghold.toml.
 * If not present or unable to parse, will read the default compiled-in
 * configuration from conf/default_config.toml.
 */
void Robot::LoadConfig() {
  auto path = "/home/lvuser/avenger.toml";
  logger_->info("loading configuration from: {}", path);

  try {
    config_ = cpptoml::parse_file(path);
    return;
  } catch (const std::exception& e) {
    logger_->warn(e.what());
  }

  logger_->warn("using default config compiled into binary");
  std::string conf(default_config_toml, std::end(default_config_toml));
  std::istringstream is{conf};
  cpptoml::parser p{is};
  config_ = p.parse();
}

void Robot::LogVersion() {
  logger_->info("Avenger {}.{}.{}{} initializing...", AVENGER_VERSION_MAJOR,
                AVENGER_VERSION_MINOR, AVENGER_VERSION_PATCH,
                AVENGER_VERSION_META);
  logger_->info(" build {} compiled at {} {}", AVENGER_VERSION_BUILD,
                AVENGER_COMPILE_DATE, AVENGER_COMPILE_TIME);
}

// used for: strings stronghold | grep AVENGER_VERSION
static const char* stronghold_version =
    "AVENGER_VERSION " AVENGER_VERSION " " AVENGER_COMPILE_DATE
    " " AVENGER_COMPILE_TIME;

START_ROBOT_CLASS(avenger::Robot)
