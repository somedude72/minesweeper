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

You will need CMake and a valid C++17 compiler toolchain. You will also need to install
the following packages to build the project:

 + Qt6 (required)
 + Spdlog (optional)

For example, this can be done on Debian distributions using the following command:

```bash
$ sudo apt install qt6-base-dev libspdlog-dev
```

Additionally, if you have installed Qt on non-standard paths, you may need to specify
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
$ cmake --install .
```

On macOS, the dynamically linked Qt and spdlog libraries are not included within the
generated .app bundle by default. Therefore, if you desire to build a redistributable
application bundle, you may have to use the `macdeployqt` tool to copy the libraries into
the bundle. 

## Notes

Note that this project is currently still in heavy development, and some aspects may not
be polished or be extremely buggy. 