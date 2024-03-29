# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

## [17.6.0]

This is the release for the 2017 Festival of Champions.

### Changed

- Camera angle setting from 56.9 to 57.4 degrees, shooter azimuth correction changed from 35 to 0 pixels and range correction from 9 to 4 inches to account for shooter and frame repairs.
- Aiming auto-azimuth control loop tuning updated.

## [17.5.0]

This is the release for IRI.

### Added

- Boiler camera azimuth and range adjustments in config file. This allows correction for any problems that occur during a competition.
- Full pit testing routine and logging for all motors.
- New autons for red and blue retrieval-zone side gear placement and 10-ball shoot.
- New auton for blue retrieval-zone size gear placement and double hopper dump down field.

### Changed

- Maximum allowed shooter wheel speed is now 900 ticks/100ms.
- Increased maximum shooting range to 240 in. and adjust lookup table for new foam.
- Adjust firing position for blue retrieval-zone side and shoot auton.

### Fixed

- Adjusted shooter elevation zero position.
- Initialize logging properly when reading from config file on robot.

## [17.4.0]

This is the release for the 2017 World Games.

### Added

- Detailed telemetry logging for camera input to robot.

### Changed

- Tuned shooter elevation range-finding routine.
- Used different network port for sending and receiving to Jetson.

### Fixed

- Close shot would run at slow speed if started, stopped and then started again while wheel was driving speed down to zero.

## [17.3.8] - 2017-04-15

Updates made during the 2017 Michigan State Games.

### Changed

- During RED hopper dump auton, drive out from alliance wall at less acute angle.
- Activate boiler camera earlier during RED and BLUE gear placement and shoot autons.
- Shooter elevation re-zeroed.

### Fixed

- During RED and BLUE hopper dump auton, drive into wall long enough for full timeout to take effect.
- Generated log file name for telemetry data.

## [17.3.0] - 2017-04-11

This is the release for the 2017 Michigan State Games.

### Added

- Autonomous routines for:

  - RED left gear (retrieval zone), run down field
  - BLUE right gear (retrieval zone), drive down field
  - RED left gear (retrieval zone), run down field
  - RED right gear (boiler), shoot

- Autonomous will move the shooter elevation down to trip the new whale-tail pin.

- Climber has a new high-speed override button.
- Ability to collect detailed plot data for shooter elevation, azimuth targeting and gear placement control loops.

### Changed

- Many improvement to autonomous routine building blocks, including tuning PID loops, control loops in the robot and the way we use encoders.
- Tuned eight RED and BLUE alliance autonomous routines for hopper dump and gears.
- Adjust and speed up gear staging operation after mechanical improvements.
- Shooter solutions adjusted for hardware refinements.

### Fixed

- Gear placement now arms the stop motion detection using absolute value of distance travel instead of speed. In rare occasions, arming would fail since average wheel speed can stay at zero if two wheels are oriented in the opposite direction of the other two.
- Zero out the drives in Talons when going through disabled. This prevents brief spurious motor activation when coming out of disabled.

## [17.2.10] - 2017-04-01

Updates made during the East Kentwood event.

### Added

- Gear pivot initially sets its home zero position using its absolute encoder but will dynamically update this zero every time the pivot is lowered to home position.
- Button to clear gear intake by running it backwards.
- Hopper drop autons run fuel intake 4 seconds after shooter starts.

### Changed

- Climber parameters changed due to hardware repair:

  - capture current from 4 to 10 A
  - ratchet voltage from 1 to 1.5 v
  - high-speed climb distance from 26,000 to 28,000 encoder ticks

- Drive tele-operated voltage-mode current limit changed from 60 to 50 A.

- Hopper drop autons had strafe distance and wait time adjusted for the field.
- Center gear placement offset.
- Close-shot hopper voltage reduced from 6 to 4v.
- Boiler and gear camera exposure to compensate for field.

## [17.2.0] - 2017-03-29

This is the release for the East Kentwood District Event on March 30, 2017.

### Added

- SmartDashboard light that signals when a gear is inside the intake.
- Gear drop and shoot auton routines.
- Gyro will automatically be adjusted for different auton robot starting orientations.
- Auton drive routines now allow multiple contiguous segments and have acceleration and deceleration parameters.
- Logging improved. Added option to send robot log messages to file. We automatically store files on a thumb drive if inserted into roboRIO. Logging is configured in config file. More informative log messages.
- Climber now activates an "electronic ratchet" (1v drive) at end of climb to keep climb scoring switch engaged.
- Drive and azimuth motor health checks. Activated by a SmartDashboard button and read from Driver Station console log.
- Brake mode activated by flight simulator controller switch.

### Changed

- Calibration for new shooter.
- Complete re-write of boiler shooting auton routines (drive out, hopper dump).
- All drive Talon PID parameters have been re-tuned for better performance in auton.
- We now use the network instead of a serial line to communicate with the Deadeye vision code running on the Jetson TX1.
- Short boiler shot now runs hopper slower.

### Fixed

- Check for no target detected condition during shooter azimuth and elevation control.

## [17.1.7] - 2017-03-11

Updates made during the St. Joe event.

### Added

- Slow down when reaching end of climb.

### Changed

- Adjusted autonomous timed maneuvers.
- Shooter will not allow elevation above encoder position 5000.

### Fixed

- Elevation motor driving into low limit.

## [17.1.0] - 2017-03-09

This is the release for the St. Joseph District Event on March 9, 2017.

### Added

- Vagrant, Ansible and cmake build system.
- Subsystems for drive, fuel intake, fuel hopper, gear handling, climber, camera and shooter.
- Serial integration with Deadeye vision system.
- Commands for tele-operated and autonomous control of subsystems.
- Autonomous routines.
- Configuration and logging.
- Configured controls for flight simulator joystick and gamepad.
- SmartDashboard buttons for wheel zero setting.
- SmartDashboard buttons for development.

[17.1.0]: https://github.com/strykeforce/steamworks/compare/db95a39e1200b61adbb51dc9eabe4e147ffd2796...v17.1.0
[17.1.7]: https://github.com/strykeforce/steamworks/compare/steamworks-17.1.0...steamworks-17.1.7
[17.2.0]: https://github.com/strykeforce/steamworks/compare/steamworks-17.1.0...steamworks-17.2.0
[17.2.10]: https://github.com/strykeforce/steamworks/compare/steamworks-17.2.0...steamworks-17.2.10
[17.3.0]: https://github.com/strykeforce/steamworks/compare/steamworks-17.2.10...steamworks-17.3.0
[17.3.8]: https://github.com/strykeforce/steamworks/compare/steamworks-17.3.0...steamworks-17.3.8
[17.4.0]: https://github.com/strykeforce/steamworks/compare/steamworks-17.3.8...steamworks-17.4.0
[17.5.0]: https://github.com/strykeforce/steamworks/compare/steamworks-17.4.0...steamworks-17.5.0
[17.6.0]: https://github.com/strykeforce/steamworks/compare/steamworks-17.5.0...steamworks-17.6.0
[unreleased]: https://github.com/strykeforce/steamworks/compare/v17.1.7...develop
