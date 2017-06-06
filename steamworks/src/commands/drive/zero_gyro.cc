#include "zero_gyro.h"

#include "robot_map.h"

using namespace steamworks::command::drive;

ZeroGyro::ZeroGyro()
    : frc::InstantCommand("ZeroGyro"), logger_(spdlog::get("command")) {}

void ZeroGyro::Initialize() {
  RobotMap::gyro->SetAngleAdjustment(0);
  auto angle_adj = -std::fmod(RobotMap::gyro->GetAngle(), 360.0);
  logger_->info("ZeroGyro setting gyro to zero ({})", angle_adj);
  RobotMap::gyro->SetAngleAdjustment(angle_adj);
}
