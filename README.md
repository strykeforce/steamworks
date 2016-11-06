# BDC Comm

The Stryke Force motor tuning tool.

## Dependencies

This repository needs to be checked out adjacent to two other repositories. Note that `2017_Pre-Season` needs to be on the **Development** branch:

```
|
|- bdc_comm (this repository)
|- 2017_Pre-Season (git@gitlab.com:strykeforce2767/2017_Pre-Season.git) Development Branch
|- 2017_Static (git@gitlab.com:strykeforce2767/2017_Static.git)
```

Compile the roboRIO shared libraries into the static library `librio.a`. We'll temporarily do this manually:

```sh
$ cd 2017_Pre-Season/Shared_Libs/Compiled_RIO/
$ make all
$ arm-frc-linux-gnueabi-ar rcs librio.a *.o
```

## Compiling

We use CMake to build this project:

```sh
$ cd bdc_comm
$ mkdir build
$ cd build
$ cmake ..
$ make
```
