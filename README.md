## Minesweeper

This is a project written using Qt/C++ to emulate the classic minesweeper game found on
earlier Windows versions. This is my first major programming project, and also my first
time learning and doing GUI programming. Because of the power of Qt, this project should
be able to be built on all major platforms. That is, unless my programming incompetency
wrote any lines of unportable code. 

## Building

You will need CMake and a valid C++17 compiler toolchain. You will also need to install
the packages listed below using your package manager (apt, homebrew, vcpkg, etc.) to build
the project:

 + Qt6 (required)
 + Spdlog (optional)

If you have installed Qt on non-standard paths (e.g. if you used Qt's official installer
instead of a package manager), you may need to specify environmental variables for CMake
to detect the libraries and header files. Otherwise, you may proceed with building the app
by the following steps. 

```bash
# On debian-linux distributions you can use the following command
$ sudo apt install qt6-base-dev libspdlog-dev
```

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

Note that the open-source distribution of Qt does not include statically linked libraries,
and the dynamically linked Qt and spdlog libraries are not included within the generated
.app bundle (macOS) or the .exe file (Windows). Therefore, if you desire to build a
redistributable application bundle or an application folder, you may have to use the
[`macdeployqt`](https://doc.qt.io/qt-6/macos-deployment.html#frameworks) or
[`windeployqt`](https://doc.qt.io/qt-6/macos-deployment.html#frameworks) tool for the
respective platforms to copy the libraries into the bundle. 