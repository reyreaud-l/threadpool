# ThreadPool [![Build Status: master](https://travis-ci.org/reyreaud-l/threadpool.svg?branch=master)](https://travis-ci.org/reyreaud-l/threadpool) [![Doc Status: master](https://readthedocs.org/projects/threadpool/badge/?version=latest)](https://threadpool.readthedocs.io/en/latest/)
Modern C++ Thread Pool

# Available features
* Multiple queue multiple workers Threadpool.
* Fetch results of tasks run in the pool with `future`.
* Fetch number of waiting/working workers.
* Hooks to monitor the threadpool (tasks/workers)

# Getting ThreadPool

## Github Releases

You can get the latest releases for Linux x86\_64 and Mac OSX from the github release page.

The latest release can be found [here](https://github.com/reyreaud-l/threadpool/releases/latest).

## Install from source
To install, run the following commands to clone the repository and install:
```bash
$ git clone git@github.com:reyreaud-l/threadpool.git
$ cd threadpool
$ mkdir build
$ cd build
$ cmake ..
$ make install
```
## Uninstall
To uninstall, run the following command in build directory.
```bash
$ make uninstall
```

# Changelog

The changelog can be found in `doc/src/changelog.rst` or on [ReadTheDoc](https://threadpool.readthedocs.io/en/latest/src/changelog.html)

# Using ThreadPool

## Documentation
Documentation is available on [ReadTheDoc](https://threadpool.readthedocs.io/en/latest/)

You will find multiples examples and a how to get started.

## Compatibility
ThreadPool is tested on these platforms/compilers:
- Linux
  - gcc: 4.9, 5, 6, 7, 8
  - clang: 3.6, 3.7, 3.8, 3.9, 4.0, 5.0, 6.0
- OSX
  - clang xcode: 7.3, 8, 8.1, 8.2, 8.3, 9, 9.1, 9.2, 9.3, 9.4, 10

# Contributing ThreadPool

## Contribute
To contribute, simply submit a PR to the repository or create and issue and I'll
take a look at it. Feel free to do whatever you want as well!

The unreleased modification are on the `staging` branch. The `master` branch
contains all the releases.

## Tests
To tests, enable option `BUILD_TESTS`:
```bash
$ cmake -DBUILD_TESTS=ON ..
```

Then the `check` target becomes available and you can build it running
```bash
$ make check
```

## Examples
To build the examples, enable option `BUILD_EXAMPLES`:
```bash
$ cmake -DBUILD_EXAMPLES=ON ..
```

Then the `examples` target becomes available and you can build it running
```bash
$ make examples
```
