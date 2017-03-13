# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

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

[17.1.7]:https://gitlab.com/strykeforce2767/steamworks/compare/v17.1.0...v17.1.7
[17.1.0]: https://gitlab.com/strykeforce2767/steamworks/compare/db95a39e1200b61adbb51dc9eabe4e147ffd2796...v17.1.0
[unreleased]: https://gitlab.com/strykeforce2767/steamworks/compare/db95a39e1200b61adbb51dc9eabe4e147ffd2796...develop
