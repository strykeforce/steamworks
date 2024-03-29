# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Calendar Versioning](http://calver.org).

## [17.2.0] - 2017-04-26

This is the release for the 2017 World Games.

### Added

- Added more tests for boiler target discrimination.
  - Minimum arc-length of contour
  - Minimum and maximum aspect ratio of bounding boxes
  - Confirm bottom and top targets are stacked
- Ability to take snapshots of target mask if targeting error

### Changed

- Log more info if targeting errors occur
- Use different network ports for sending and receiving to robot

### Fixed

- Startup scripts were running two copies of program on Jetson.

## [17.1.0] - 2017-03-29

This is the release for the East Kentwood District Event on March 30, 2017.

### Added

- Measurement and logging of average FPS.
- Support for both boiler and gear cameras. We now can switch between cameras when commanded over the networks by the roboRIO.

### Changed

- We reverted to using the network instead of a serial line to communicate with the robot code running on the roboRIO.
- Stopped using aspect ratio for gear target detections since the position of our camera cuts off the bottom of the target at close range.

## [17.0.0] - 2017-03-08

This is the release for the St. Joseph District Event on March 9, 2017.

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

[16.0.0]: https://github.com/strykeforce/deadeye/compare/09a40e5c4c53778fee66cdf748963c708f3e2305...v16.0.0
[16.1.0]: https://github.com/strykeforce/deadeye/compare/v16.0.0...v16.1.0
[17.0.0]: https://github.com/strykeforce/deadeye/compare/v16.1.0...v17.0.0
[17.1.0]: https://github.com/strykeforce/deadeye/compare/v17.0.0...v17.1.0
[17.2.0]: https://github.com/strykeforce/deadeye/compare/v17.1.0...v17.2.0
[chameleon3]: https://www.ptgrey.com/chameleon3-usb3-vision-cameras
[unreleased]: https://github.com/strykeforce/deadeye/compare/v17.0.0...develop
