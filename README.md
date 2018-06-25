# ThreadPool
Modern C++ Thread Pool

# Available features
* Fixed size pool.
* Variable size pool.
* Fetch result of task run in the pool with `future`.
* Fetch number of waiting/working workers.

# Install
To install, run the following commands
```bash
$ mkdir build
$ cd build
$ make install
```

# Uninstall
To uninstall, run the following command in build directory.
```bash
$ make uninstall
```

# Tests
To tests you need gtest library installed and then run the following commands in
build directory.
```bash
$ make check
```
