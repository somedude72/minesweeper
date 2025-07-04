## Overview
<p float="left">
  <img width="253" alt="Minesweeper 9x9" src="https://raw.githubusercontent.com/somedude72/minesweeper/refs/heads/main/docs/dead-9x9.png" />
  <img width="530" alt="Minesweeper 9x20" src="https://raw.githubusercontent.com/somedude72/minesweeper/refs/heads/main/docs/dead-9x20.png" />
</p>

This is a cross-platform project written using Qt6/C++ to replicate the classic minesweeper game on Windows 95. The game supports custom board sizes and mine counts, safe/clear first reveal, custom seeds, and more. 

## Building

You will need CMake and a valid C++20 compiler toolchain. You will also need to install the packages listed below using your package manager (apt, homebrew, vcpkg, etc.) to build the project:

 + Qt6 (required)
 + Fmt (required)
 + Spdlog (optional)

```bash
# On debian-linux distributions you can use the following command
$ sudo apt install qt6-base-dev libspdlog-dev libfmt-dev
```

If you have installed Qt on non-standard paths (e.g. if you used Qt's official installer instead of a package manager), you may need to specify environmental variables for CMake to detect the libraries and header files. Otherwise, you may proceed with building the app by the following steps. 

 + Clone the repository using git

```bash
$ git clone https://github.com/somedude72/minesweeper.git
$ cd minesweeper
```
 + Make a build folder and build using CMake

```bash
$ mkdir build && cd build
$ cmake .. && cmake --build . 
$ cmake --install .
```

 + The executable should be visible in the build directory

Note that the open-source distribution of Qt does not provide statically linked libraries, and the dynamically linked libraries are not included within the generated app bundle (macOS) or the exe file (Windows) by default. Therefore, if you wish to build a redistributable application bundle or an application folder, you may have to use the [`macdeployqt`](https://doc.qt.io/qt-6/macos-deployment.html#frameworks) or [`windeployqt`](https://doc.qt.io/qt-6/macos-deployment.html#frameworks) tool for the respective platforms to copy the libraries (`.dylib`, `.dll`, etc.) into the bundle. 

## Support

Consider starring this repository to support this project.