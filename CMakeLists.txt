cmake_minimum_required(VERSION 3.21.1 FATAL_ERROR)
project(minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

# Packages
set(LIBRARIES Qt6::Core Qt6::Gui Qt6::Widgets)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(spdlog QUIET)

if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found: disabling codebase logging")
    add_compile_definitions(NO_SPDLOG)
else()
    message(STATUS "spdlog found: enabling codebase logging")
    set(LIBRARIES ${LIBRARIES} spdlog::spdlog)
endif()

# Minesweeper app
add_subdirectory(src)
target_include_directories(minesweeper PRIVATE src)
target_link_libraries(minesweeper PRIVATE ${LIBRARIES})

set_target_properties(minesweeper PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER com.zichen.minesweeper
    MACOSX_BUNDLE_BUNDLE_NAME minesweeper
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
)


install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)