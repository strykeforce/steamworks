#pragma once

#include <AHRS.h>
#include <cpptoml/cpptoml.h>

#include <sidewinder/swerve/talon_map.h>

namespace steamworks {

class RobotMap {
 public:
  enum Talons {
    kRightFrontAzimuth = 2,
    kRightFrontDrive = 12,
    kLeftFrontAzimuth = 1,
    kLeftFrontDrive = 11,
    kRightRearAzimuth = 3,
    kRightRearDrive = 13,
    kLeftRearAzimuth = 4,
    kLeftRearDrive = 14,
    kShooterWheel = 21,
    kShooterElevation = 22,
    kHopper = 31,
    kIntake = 41,
    kGearIntake = 51,
    kGearPivot = 52,
    kClimberMaster = 61,  // reversed
    kClimberSlave = 62,   // counts forward during climb
  };

  enum Servos {
    kGearClampLeft = 0,
    kGearClampRight = 1,
  };

  enum DigitalInputs {
    kAutonSwitchLSB = 0,
    kAutonSwitchMSB = 6,
    kPracticeRobot = 7,
  };

  enum DigitalOutputs {
    kGearCameraLight = 8,
    kShooterCameraLight = 9,
  };

  // pointers to talons and azimuth encoder zero count
  static sidewinder::swerve::TalonMap* swerve_talons;

  static ::CANTalon* shooter_wheel_talon;
  static ::CANTalon* shooter_elevation_talon;
  static ::CANTalon* hopper_talon;
  static ::CANTalon* intake_talon;
  static ::CANTalon* gear_intake_talon;
  static ::CANTalon* gear_pivot_talon;
  static ::CANTalon* climber_master_talon;
  static ::CANTalon* climber_slave_talon;

  // pointer to navX gyro singleton
  static std::shared_ptr<AHRS> gyro;

  // pointers to camera LED digital control channels
  static std::shared_ptr<frc::DigitalOutput> gear_camera_led;
  static std::shared_ptr<frc::DigitalOutput> shooter_camera_led;

  // instantiate talons and read encoder zero count
  static void Initialize(const std::shared_ptr<cpptoml::table> config);

  // tells which robot we're running on
  static bool IsPracticeRobot();
};

} /* steamworks */
