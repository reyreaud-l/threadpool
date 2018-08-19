# ThreadPool
Modern C++ Thread Pool

# Available features
* Fixed size pool.
* Variable size pool.
* Fetch result of task run in the pool with `future`.
* Fetch number of waiting/working workers.

# Install
To install, run the following commands to clone the repository and install
```bash
$ git clone git@github.com:reyreaud-l/threadpool.git
$ cd threadpool
$ mkdir build
$ cd build
$ cmake ..
$ make install
```

# Doc
Documentation is available on [ReadTheDoc](https://threadpool.readthedocs.io/en/latest/)

You will find multiples examples and a how to get started.

# Uninstall
To uninstall, run the following command in build directory.
```bash
$ make uninstall
```

# Tests
To tests, enable option `BUILD_TESTS` or build in `DEBUG` mode with either:
```bash
$ cmake -DBUILD_TESTS=ON ..
or
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Then the `check` target becomes available and you can build it running
```bash
$ make check
```
