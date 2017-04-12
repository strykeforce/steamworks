# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

## [17.4.0] - 2017-04-12

The 2017 Michigan State Games release.

### Added

- Function to take average of front two wheels to accommodate rear wheel skidding when stopping against gear lift.

### Changed

- Take average of all four wheels when getting drive encoder position in `GetPosition()`.

## [17.3.0] - 2017-03-29

This is the release for the East Kentwood District Event on March 30, 2017.

### Added

- Support for brake mode for swerve drive.
- Support for motor health checks.
- Ability to clear integrator for drive motors.
- Ability to zero out encoders for drive motors to allow distance measurements.

### Changed

- Allow manual control of drive dead-zone.
- Now allow programatic change of gyro orientation to accomodate different auton starting orientations.

### Fixed

## [17.2.0] - 2017-03-08

This is the release for the St. Joseph District Event on March 9, 2017.

### Added

- Implement off-center pivot of robot to enable vision-based shooter aiming, from @ztannhauser. (#19)
- Implement field-oriented gyro drive mode from @ztannhauser. (#17)
- Provide version information for Sidewinder library users. (#14)
- Added new Talon current limit and motion magic settings to settings.
- Swerve drive wheel zero settings are now saved in settings file in `~lvuser/sidewinder_calibration.toml`. This allows saving settings using SmartDashboard button.
- Convenience methods for autonomous driving.

### Changed

- SwerveDrive class now implements frc::Subsystem to allow direct use by robots that implement command-based programming. (#29)
- Improve checks for missing Talon configuration settings in config file. (#10)
- Setpoint scaling for different motor control modes is now settable per Talon configuration instead of once at top level.

### Fixed

- SwerveDrive drive command dead-zone is now configurable instead of being hard-coded into the library. (#4)
- Compiling library no longer generates compiler warnings from grapher code. (#21)

## [17.1.0] - 2017-01-30

### Added

- Better config file error checking. (#10)
- Unit tests in `test` directory.
- Convenience method to initialize Talons.
- Configurable swerve wheelbase dimensions. (#18)
- Grapher support moved to library. (#15)

### Changed

- Swerve drive now in `swerve` namespace. (#6)

### Fixed

- Azimuth reset when crossing negative Y-axis (#3)
- More updates to conform with 2017 WPILib changes. (#11)

## [17.0.0] - 2017-01-06

Version 16.0.0 updated for 2017 libraries.

### Changed

- WPILib now in `frc` namespace.
- Talon code broken out into separate CTRE library.

## [16.0.0] - 2017-01-06

This is the last feature release for 2016.

### Added

- Velocity PID mode
- Expo input filter
- Simple Moving Average input filter

## [16.0.0-alpha] - 2016-11-21

This releases supports the swerve drive code for the Avenger t-shirt cannon.

### Added

- Initial release of functional swerve drive.

[16.0.0]: https://github.com/strykeforce/sidewinder/compare/v16.0.0-alpha...v16.0.0
[16.0.0-alpha]: https://github.com/strykeforce/sidewinder/compare/v16.0.0-dev...v16.0.0-alpha
[17.0.0]: https://github.com/strykeforce/sidewinder/compare/v16.0.0...v17.0.0
[17.1.0]: https://github.com/strykeforce/sidewinder/compare/v17.0.0...v17.1.0
[17.2.0]: https://github.com/strykeforce/sidewinder/compare/v17.1.0...v17.2.0
[17.3.0]: https://github.com/strykeforce/sidewinder/compare/v17.2.0...v17.3.0
[17.4.0]: https://github.com/strykeforce/sidewinder/compare/v17.3.0...v17.4.0
[unreleased]: https://github.com/strykeforce/sidewinder/compare/v17.2.0...develop
