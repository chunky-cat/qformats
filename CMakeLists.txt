cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

project(
    qformats
    VERSION 1.0
    LANGUAGES CXX
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMACOSX)
endif()

add_subdirectory(thirdparty)
add_subdirectory(qformats)
add_subdirectory(qmap_viewer)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
