# Sidewinder

The Stryke Force swerve drive library. It provides a WPILib command-based subsystem in `SwerveDrive` and example command classes. There is an example robot in the `examples` directory that will drive the 2016 Protobot swerve drive.

# Installation

This will install the Sidewinder static library and necessary header files in `/opt/sidewinder`.

Vagrant is recommended as it will provision the correct development environment automatically. Note: installation requires a Gitlab deployment password to be in the `.vault_pw` file at the top of the repo. Contact a team member to obtain it.

```sh
$ cd sidewinder
$ vagrant up
$ vagrant ssh
$ # now logged into virtual build environment
$ cd /vagrant
```

To compile:

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

To install:

```sh
$ sudo make install
```

To run tests:

```sh
$ cd test
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./test-runner
```
