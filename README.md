# BDC Comm

The Stryke Force motor tuning tool.

You will need [Vagrant](https://www.vagrantup.com) installed. Vagrant is used to build as it will provision the correct development environment automatically.

To set up the build environment:

```sh
$ # ensure vagrant is installed per above
$ cd bdc_comm
$ vagrant up
$ # virtual machine image downloaded, booted and provisioned
$ vagrant ssh
$ # now logged into virtual build environment
$ cd /vagrant
```

We use CMake to build this project:

```sh
$ # ssh'd into virtual build environment and in /vagrant directory
$ mkdir build
$ cd build
$ # set up build
$ cmake ..
$ # run make to compile
$ make
$ # from now on, just run make to rebuild
```
