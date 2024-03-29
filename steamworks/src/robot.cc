#include "robot.h"

#include <WPILibVersion.h>
#include <sidewinder/version.h>
#include <cmath>

#include "commands/auton/auton.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "default_config.h"
#include "log.h"
#include "oi.h"
#include "robot_map.h"
#include "subsystems/climber.h"
#include "subsystems/deadeye.h"
#include "subsystems/drive.h"
#include "subsystems/gear.h"
#include "subsystems/hopper.h"
#include "subsystems/intake.h"
#include "subsystems/shooter_elevation.h"
#include "subsystems/shooter_wheel.h"
#include "version.h"

using namespace steamworks;
using namespace steamworks::command;

OI* Robot::oi = nullptr;
subsystem::Deadeye* Robot::deadeye = nullptr;
subsystem::Climber* Robot::climber = nullptr;
subsystem::SwerveDrive* Robot::drive = nullptr;
subsystem::GearLoader* Robot::gear_loader = nullptr;
subsystem::Hopper* Robot::hopper = nullptr;
subsystem::Intake* Robot::intake = nullptr;
subsystem::ShooterElevation* Robot::shooter_elevation = nullptr;
subsystem::ShooterWheel* Robot::shooter_wheel = nullptr;

Robot::Robot() : frc::IterativeRobot() {}

void Robot::RobotInit() {
  Configure();
  RobotMap::Initialize();

  SPDLOG_TRACE(logger_, "initializing subsystems");
  logger_->info("running on {} robot",
                RobotMap::IsPracticeRobot() ? "PRACTICE" : "COMPETITION");
  deadeye = new subsystem::Deadeye(config_);
  deadeye->Start();
  climber = new subsystem::Climber(config_);
  drive = new subsystem::SwerveDrive(config_);
  gear_loader = new subsystem::GearLoader(config_);
  hopper = new subsystem::Hopper(config_);
  intake = new subsystem::Intake(config_);
  shooter_elevation = new subsystem::ShooterElevation(config_);
  shooter_wheel = new subsystem::ShooterWheel(config_);
  SPDLOG_TRACE(logger_, "done initializing subsystems");
  oi = new OI(config_);  // keep this after subsystems
}

void Robot::RobotPeriodic() {}

void Robot::DisabledInit() {
  logger_->info("disabled, flushing logs...");
  Log::GetInstance().Flush();
}

void Robot::DisabledPeriodic() {
  drive->SetDrive(0.0);
  drive->ClearDriveIaccum();
  frc::Scheduler::GetInstance()->Run();
}

void Robot::AutonomousInit() {
  RobotMap::gyro->SetAngleAdjustment(0);
  auto angle_adj = -std::fmod(RobotMap::gyro->GetAngle(), 360.0);
  logger_->info("AutonomousInit setting gyro to zero ({})", angle_adj);
  RobotMap::gyro->SetAngleAdjustment(angle_adj);

  auto auton_mode = oi->GetAutonMode();
  logger_->info("AutonomousInit initialize auton mode {:X}", auton_mode);
  switch (auton_mode) {
    case 1:
      // BLUE alliance hopper dump and shoot
      autonomous_command_ = new auton::Sequence01();
      gyro_offset_ = -90.0;
      break;
    case 2:
      // RED alliance hopper dump and shoot
      autonomous_command_ = new auton::Sequence02();
      gyro_offset_ = 90.0;
      break;
    case 3:
      // Test
      autonomous_command_ = new auton::Sequence03();
      gyro_offset_ = 0;
      break;
    case 4:
      // BLUE alliance drive out and shoot
      autonomous_command_ = new auton::Sequence04();
      gyro_offset_ = 0;
      break;
    case 5:
      // RED alliance drive out and shoot
      autonomous_command_ = new auton::Sequence05();
      gyro_offset_ = 0;
      break;
    case 6:
      // BLUE alliance center gear and shoot
      autonomous_command_ = new auton::Sequence06();
      gyro_offset_ = 0;
      break;
    case 7:
      // RED alliance center gear and shoot
      autonomous_command_ = new auton::Sequence07();
      gyro_offset_ = 0;
      break;
    case 8:
      // BLUE alliance left gear and shoot
      autonomous_command_ = new auton::Sequence08();
      gyro_offset_ = 0;
      break;
    case 9:
      // BLUE alliance right gear and shoot
      autonomous_command_ = new auton::Sequence09();
      gyro_offset_ = 0;
      break;
    case 10:
      // BLUE alliance right gear and run down field - 0A
      autonomous_command_ = new auton::Sequence0A();
      gyro_offset_ = 0;
      break;
    case 11:
      // RED alliance left gear and run down field - 0B
      autonomous_command_ = new auton::Sequence0B();
      gyro_offset_ = 0;
      break;
    case 12:
      // RED alliance right gear and shoot - 0C
      autonomous_command_ = new auton::Sequence0C();
      gyro_offset_ = 0;
      break;
    case 13:
      // BLUE alliance right gear and dump 2 hoppers - 0D
      autonomous_command_ = new auton::Sequence0D();
      gyro_offset_ = 0;
      break;
    case 14:
      // RED alliance left gear and shoot - 0E
      autonomous_command_ = new auton::Sequence0E();
      gyro_offset_ = 0;
      break;
    default:
      autonomous_command_ = new LogCommand("unrecognized command");
  }
  shooter_elevation->SetElevation(0);
  autonomous_command_->Start();
}

void Robot::AutonomousPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  auto angle_adj = RobotMap::gyro->GetAngleAdjustment() + gyro_offset_;
  logger_->info("AutonomousInit offsetting gyro {} deg. to zero ({})",
                gyro_offset_, angle_adj);
  RobotMap::gyro->SetAngleAdjustment(angle_adj);

  frc::Joystick fsj(OI::kFlightSimJoystick);
  if (fsj.GetRawButton(OI::kFlightSimLeftCornerDownButton)) {
    // button is already in auto on position so run command
    logger_->info("TeleopInit auto gear load is on");
  }

  if (fsj.GetRawButton(OI::kFlightSimLeftCornerUpButton)) {
    // button is already in auto on position so run command
    logger_->info("TeleopInit auto gear load is off");
  }

  // reset the gyro for driving

  stop_shooter_ = new StopShooting();
  stop_shooter_->Start();
}

void Robot::TeleopPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { SPDLOG_TRACE(logger_, "in TestInit"); }

void Robot::TestPeriodic() { frc::LiveWindow::GetInstance()->Run(); }

/**
 Reads our configuration file from ~lvuser/steamworks.toml.
 * If not present or unable to parse, will read the default compiled-in
 * configuration from conf/default_config.toml.
 */
void Robot::Configure() {
  auto path = "/home/lvuser/steamworks.toml";
  // configure interim logger
  logger_ = spdlog::stdout_logger_st("config");
  LogVersion();
  logger_->info("loading configuration from: {}", path);

  try {
    config_ = cpptoml::parse_file(path);
    Log::GetInstance().Initialize(config_);
    logger_ = spdlog::get("robot");
    spdlog::drop("config");
    return;
  } catch (const std::exception& e) {
    logger_->warn(e.what());
  }

  logger_->warn("using default config compiled into binary");
  std::string conf(default_config_toml, std::end(default_config_toml));
  std::istringstream is{conf};
  cpptoml::parser p{is};
  config_ = p.parse();
  Log::GetInstance().Initialize(config_);
  logger_ = spdlog::get("robot");
  spdlog::drop("config");
}

/**
 * Log the build version.
 */
void Robot::LogVersion() {
  logger_->info("STEAMWORKS {}.{}.{}{} initializing...",
                STEAMWORKS_VERSION_MAJOR, STEAMWORKS_VERSION_MINOR,
                STEAMWORKS_VERSION_PATCH, STEAMWORKS_VERSION_META);
  logger_->info(" build {} compiled at {} {}", STEAMWORKS_VERSION_BUILD,
                STEAMWORKS_COMPILE_DATE, STEAMWORKS_COMPILE_TIME);
  logger_->info(sidewinder::GetVersion());
  logger_->info("WPILib version: {}", WPILibVersion);
#ifdef NDEBUG
  logger_->info("configured as RELEASE build");
#else
  logger_->warn("configured as DEBUG build");
#endif
}

// used for: strings steamworks | grep STEAMWORKS_VERSION
static const char* steamworks_version =
    "STEAMWORKS_VERSION " STEAMWORKS_VERSION " " STEAMWORKS_COMPILE_DATE
    " " STEAMWORKS_COMPILE_TIME;

START_ROBOT_CLASS(steamworks::Robot)
