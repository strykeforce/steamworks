# Stryke Force 2767 STEAMWORKS

The Stryke Force robot for the 2017 STEAMWORKS season.

## Prerequisites

You will need [VirtualBox](https://www.virtualbox.org) and [Vagrant](https://www.vagrantup.com) installed. Vagrant is used to build as it will automatically provision a VirtualBox virtual machine (VM) with the correct tools and dependencies.

## Installation

With Vagrant, the `steamworks` repository (this directory) will be shared in the VM as `/vagrant`. Edit files as normal on your host machine and changes are visible in the build VM.

To set up the build environment:

```sh
$ # ensure vagrant is installed per above
$ cd steamworks
$ vagrant up
$ # virtual machine image downloaded, booted and provisioned
$ vagrant ssh
$ # now logged into virtual build environment
$ cd /vagrant
```

We use CMake to build this project:

```sh
$ # after ssh'd into virtual build environment and in /vagrant directory
$ mkdir build
$ cd build
$ # set up build
$ cmake ..
$ # run make to compile
$ make
$ # from now on, just run make to rebuild
```

A normal programming session looks like:

1. Start the Vagrant VM with `vagrant up`.
2. Edit.
3. Log into the VM with `vagrant ssh`.
4. Compile.
5. Deploy the compiled executable to the robot for testing.
6. When finished, suspend the Vagrant VM with `vagrant halt`.
