# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

## [Unreleased]

### Added

- Measurement and logging of average FPS.

### Changed

- We now use the network instead of a serial line to communicate with the robot code running on the roboRIO.

## [17.0.0] - 2017-03-08

The St. Joseph Districts release.

### Added

- We support multiple targeting modes using difference cameras. Implemented modes for STEAMWORKS boiler and gear targeting.
- Generate robot boiler shooting solutions from a Excel lookup table. Shooter elevation and wheel speed are selected by boiler target pixel vertical separation.
- Added support for the FLIR USB3 [Chameleon3] camera for boiler shooter.
- Added support for the Microsoft LifeCam for gear targeting support.

### Changed

- TTL serial is now used instead of UDP for communication with the roboRIO.

## [16.1.0] - 2016-10-28

Rewrite for StrongHold, tested very lightly.

### Changed

- Refactored into classes.
- Use HSV for image processing.

## [16.0.0] - 2016-10-08

StrongHold competition code.

[16.0.0]: https://gitlab.com/strykeforce2767/deadeye/compare/09a40e5c4c53778fee66cdf748963c708f3e2305...v16.0.0
[16.1.0]: https://gitlab.com/strykeforce2767/deadeye/compare/v16.0.0...v16.1.0
[17.0.0]: https://gitlab.com/strykeforce2767/deadeye/compare/v16.1.0...v17.0.0
[chameleon3]: https://www.ptgrey.com/chameleon3-usb3-vision-cameras
[unreleased]: https://gitlab.com/strykeforce2767/deadeye/compare/v17.0.0...develop
