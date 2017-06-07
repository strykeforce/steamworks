# Stryke Force C++ Robots

This repository contains currently maintained robots and supporting libraries that were written in C++. They are kept in a single repository since they have shared code. Each robot project listed below is a stand-alone project.

## STEAMWORKS

The Stryke Force robot for the 2017 STEAMWORKS season.

## Avenger

The Stryke Force t-shirt cannon.

## Configuring the Development Environment

You will need [VirtualBox](https://www.virtualbox.org) and [Vagrant](https://www.vagrantup.com) installed. Vagrant is used to build as it will automatically provision a VirtualBox virtual machine (VM) with the correct tools and dependencies.

In order to allow installation of our private resources you will also need a Gitlab access password to be in a file named `.vault_pw` in the root directory of this repository on your development computer. Contact a team member to obtain it.

## Installation

With Vagrant, the `robots-cpp` repository (this directory) will be shared in the VM as `/vagrant`. Edit files as normal on your host machine and changes are visible in the build VM.

To set up the build environment:

```sh
$ # ensure vagrant is installed per above
$ cd robots-cpp
$ vagrant up
$ # virtual machine image downloaded, booted and provisioned
$ vagrant ssh
$ # now logged into virtual build environment, go to robot project directory
$ cd /vagrant/steamworks
```

We use CMake to build this project:

```sh
$ # after ssh'd into virtual build environment and in robot directory
$ # for example in /vagrant/steamworks
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
