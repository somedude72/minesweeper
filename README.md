## Minesweeper

This is a project written using Qt/C++ to emulate the classic minesweeper game found on
earlier Windows versions. 

## Building

#### Windows

This project has not been tested on Windows. Although the code should be completely
portable in theory because of Qt, it has never been built on Windows. Therefore, this
section will remain under construction until I get my hands on a Windows machine to test
drive building minesweeper.

#### macOS/Linux

You will need a valid C++17 compiler toolchain to build this program (e.g. the 
`build-essential` package on most linux distributions). You will also need to have the
following libraries/packages installed on your system through your preferred package
manager:

 + `CMake`
 + `spdlog`
 + `Qt6`

For example, this can be done on Debian distributions using the following command:

```bash
$ sudo apt install cmake libspdlog-dev qt6-base-dev
```

If you have installed them on non-standard paths, you may need to specify additional
environmental variables for CMake to detect the libraries and header files. Otherwise, you
may proceed with building the app by the following steps:

 + Clone the repository
```bash
$ git clone https://github.com/somedude72/minesweeper.git
$ cd minesweeper
```
 + Make a build folder and build

```bash
$ mkdir build && cd build
$ cmake .. && cmake --build .
```

 + The resulting executable should be visible at `build/src/minesweeper`.

## Notes

Note that this project is currently still in heavy development, and some aspects may not
be polished or be extremely buggy. 