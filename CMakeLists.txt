######################
## Project settings ##
######################


cmake_minimum_required(VERSION 3.21.1 FATAL_ERROR)
project(Minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

# set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)


#######################
## Project libraries ##
#######################


set(LIBRARIES)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
set(LIBRARIES ${LIBRARIES} Qt6::Core Qt6::Gui Qt6::Widgets)

# Spdlog
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "Library spdlog NOT found: disabling codebase logging")
    add_compile_definitions(NO_SPDLOG)
else()
    message(STATUS "Library spdlog found: enabling codebase logging")
    set(LIBRARIES ${LIBRARIES} spdlog::spdlog)
endif()


#####################
## Project sources ##
#####################

set(SOURCES
    src/main.cpp
    src/app/app.cpp
    src/model/data.cpp
    src/view/button.cpp
    src/view/window.cpp
    src/view/window.ui
)

####################
## Project assets ##
####################

set(ASSETS
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


#################################
## Project include directories ##
#################################


set(INCLUDE_DIRS src)


######3#############
## Project target ##
####################


qt_add_executable(${PROJECT_NAME} ${SOURCES})
qt_add_resources(${PROJECT_NAME} "assets" PREFIX "/" FILES ${ASSETS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/assets/macos/Info.plist
)

set_source_files_properties(${ASSETS}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/"
)

# install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)