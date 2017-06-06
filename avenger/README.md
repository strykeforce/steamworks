# Avenger

The Stryke Force t-shirt cannon.

# Installation

Vagrant is recommended as it will provision the correct development environment automatically. Note: installation requires a Gitlab deployment password to be in the `.vault_pw` file at the top of the repo. Contact a team member to obtain it.

```sh
$ cd avenger
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
