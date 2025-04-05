cmake_minimum_required(VERSION 3.21.1 FATAL_ERROR)
project(minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})


set(LIBRARIES Qt6::Core Qt6::Gui Qt6::Widgets)

# Packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(spdlog QUIET)

if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found: disabling codebase logging")
    add_compile_definitions(NO_SPDLOG)
else()
    message(STATUS "spdlog found: enabling codebase logging")
    set(LIBRARIES ${LIBRARIES} spdlog::spdlog)
endif()

# Minesweeper target
qt_add_executable(minesweeper
    src/main.cpp
    src/app/app.cpp
    src/view/button.cpp
    src/view/game.cpp
    src/view/game.ui
    src/model/data.cpp
)

qt_add_resources(minesweeper "assets"
    PREFIX "/"
    FILES
        assets/ms-dead.png
        assets/ms-win.png
        assets/ms-smile.png
        assets/ms-flag.png
        assets/ms-cross.png
        assets/ms-mine.png
        assets/ms-numbers.ttf
)

# Target configuration
target_include_directories(minesweeper PRIVATE src)
target_link_libraries(minesweeper PRIVATE ${LIBRARIES})

set_target_properties(minesweeper PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

set_source_files_properties(${ASSETS}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/"
)

install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)