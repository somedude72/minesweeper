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

```bash
# On debian-based distributions you can use the following command
$ sudo apt install qt6-base-dev libspdlog-dev
```

Note that if you have installed Qt on non-standard paths, you may need to specify
environmental variables for CMake to detect the libraries and header files. Otherwise, you
may proceed with building the app by the following steps. 

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

Note that this project is currently still in heavy development, and some aspects may be
non-functional or buggy. 

Please also note that there is a (known) issue that the performance might be poor when
when revealing a large number of squares. This issue originates with Qt's slow stylesheet
updates and is not easily fixable by me. However, I have done my best to limit the
stylesheet refreshes such that the lag should be barely noticeable. 