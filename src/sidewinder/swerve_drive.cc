#include "swerve_drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "talon/position_talon.h"
#include "talon_map.h"
#include "tele_drive.h"

namespace sidewinder {

/** Initialize the Sidewinder tele drive command.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new TeleDrive(oi_, this));
}

/** Initialize the Sidewinder SwerveDrive.
 * @param config cpptoml SIDEWINDER table
 * @param tm talon map initialized with pointers to drive talons
 * @oi sidewinder-specific operator input
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config,
                         const TalonMap* tm, const OI* oi)
    : Subsystem("SwerveDrive"),
      logger_(spdlog::stdout_color_st(GetName())),
      map_(tm),
      oi_(oi) {
  logger_->set_level(spdlog::level::trace);
  logger_->trace("starting constructor");

  auto azimuth_cfg_data = config->get_table("AZIMUTH");
  assert(azimuth_cfg_data);
  auto azimuth_talon_cfg = talon::PositionTalon(azimuth_cfg_data);
  logger_->debug("dumping azimuth talon configuration");
  azimuth_talon_cfg.LogConfig(logger_);

  logger_->trace("configuring talons");
  azimuth_talon_cfg.Configure(map_->lf_azimuth);
  azimuth_talon_cfg.SetMode(map_->lf_azimuth);
  azimuth_talon_cfg.Configure(map_->rf_azimuth);
  azimuth_talon_cfg.SetMode(map_->rf_azimuth);
  azimuth_talon_cfg.Configure(map_->lr_azimuth);
  azimuth_talon_cfg.SetMode(map_->lr_azimuth);
  azimuth_talon_cfg.Configure(map_->rr_azimuth);
  azimuth_talon_cfg.SetMode(map_->rr_azimuth);

  SetEncoderZero(azimuth_cfg_data);

  logger_->trace("done with constructor");
}

void SwerveDrive::SetEncoderZero(const std::shared_ptr<cpptoml::table> config) {
  // Encoder has 12-bit resolution so mask with 0xFFF. Add 4096 in case current
  // encoder position is less than zero position. We mask delta again for cases
  // where current encoder position was > zero.
  // int abs_pos = (talon->GetPulseWidthPosition() & 0xFFF) + 0xFFF;
  logger_->trace("setting azimuth zero");

  auto pos = (map_->lf_azimuth->GetPulseWidthPosition() -
                 *config->get_as<int>("lf_zero")) &
             0xFFF;
  map_->lf_azimuth->SetEncPosition(pos);
  logger_->debug("set left front azimuth position = {}", pos);

  pos = (map_->rf_azimuth->GetPulseWidthPosition() -
            *config->get_as<int>("rf_zero")) &
        0xFFF;
  map_->rf_azimuth->SetEncPosition(pos);
  logger_->debug("set right front azimuth position = {}", pos);

  pos = (map_->lr_azimuth->GetPulseWidthPosition() -
            *config->get_as<int>("lr_zero")) &
        0xFFF;
  map_->lr_azimuth->SetEncPosition(pos);
  logger_->debug("set left rear azimuth position = {}", pos);

  pos = (map_->rr_azimuth->GetPulseWidthPosition() -
         *config->get_as<int>("rr_zero")) &
        0xFFF;
  map_->rr_azimuth->SetEncPosition(pos);
  logger_->debug("set right rear azimuth position = {}", pos);
}

/** Move all wheels to their home position.
 */
void SwerveDrive::ZeroAzimuth() {
  map_->lf_azimuth->Set(0.0);
  map_->rf_azimuth->Set(0.0);
  map_->lr_azimuth->Set(0.0);
  map_->rr_azimuth->Set(0.0);
}

/** Drive in crab drive mode.
 * @param forward command forward/backwards (Y-axis) motion
 * @param strafe command left/right (X-axis) motion
 */
void SwerveDrive::CrabDrive(float forward, float strafe) {
  logger_->debug("crab driving forward = {}, azimith = {}", forward, strafe);
}

} /* sidewinder */
