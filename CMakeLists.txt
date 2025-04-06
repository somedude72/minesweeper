cmake_minimum_required(VERSION 3.21.1 FATAL_ERROR)
project(Minesweeper LANGUAGES CXX)

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
qt_add_executable(Minesweeper
    src/main.cpp
    src/app/app.cpp
    src/view/button.cpp
    src/view/window.cpp
    src/view/window.ui
    src/model/data.cpp
)

qt_add_resources(Minesweeper "assets"
    PREFIX "/"
    FILES
        assets/board/dead.png
        assets/board/win.png
        assets/board/revealing.png
        assets/board/smile.png

        assets/board/flag.png
        assets/board/cross.png
        assets/board/mine.png
        assets/board/font.ttf

        assets/window/close.png
        assets/window/minimize.png
        assets/window/font.otf
        
)

# Target configuration
target_include_directories(Minesweeper PRIVATE src)
target_link_libraries(Minesweeper PRIVATE ${LIBRARIES})

set_target_properties(Minesweeper PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/macos/Info.plist
)

set_source_files_properties(${ASSETS}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/"
)

install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)