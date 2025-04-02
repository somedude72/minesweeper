cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_subdirectory(src)
target_include_directories(minesweeper PUBLIC src)
target_link_libraries(minesweeper PUBLIC Qt::Core Qt::Gui Qt::Widgets)
set_target_properties(minesweeper PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)