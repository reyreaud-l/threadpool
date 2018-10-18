# ThreadPool [![Build Status: master](https://travis-ci.org/reyreaud-l/threadpool.svg?branch=master)](https://travis-ci.org/reyreaud-l/threadpool) [![Doc Status: master](https://readthedocs.org/projects/threadpool/badge/?version=latest)](https://threadpool.readthedocs.io/en/latest/)
Modern C++ Thread Pool

# Available features
* Fixed size pool.
* Variable size pool.
* Fetch results of tasks run in the pool with `future`.
* Fetch number of waiting/working workers.
* Hooks to monitor the threadpool (tasks/workers)

# Getting ThreadPool

## Install
To install, run the following commands to clone the repository and install
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

# Using ThreadPool

## Documentation
Documentation is available on [ReadTheDoc](https://threadpool.readthedocs.io/en/latest/)

You will find multiples examples and a how to get started.

## Compatibility
ThreadPool is tested on these platforms/compilers:
- Linux
  - gcc
    - 4.9
    - 5
    - 6
    - 7
  - clang
    - 3.6
    - 3.7
    - 3.8
    - 3.9
    - 4.0
    - 5.0
- OSX
  - gcc
    - 5
    - 6
    - 7
  - clang xcode
    - 8.3
    - 9.1

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
