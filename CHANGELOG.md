# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

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
[17.1.7]: https://github.com/strykeforce/steamworks/compare/v17.1.0...v17.1.7
[17.2.0]: https://github.com/strykeforce/steamworks/compare/v17.1.0...v17.2.0
[unreleased]: https://github.com/strykeforce/steamworks/compare/v17.1.7...develop
