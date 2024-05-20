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

find_package(OpenGL REQUIRED) # error

add_library(${PROJECT_NAME} SHARED
    src/common/textureman.cpp    
    src/map/map_file.cpp
    src/map/map.cpp
    src/map/brush.cpp
    src/map/qmath.cpp
    src/map/polygon.cpp
    src/wad/palette.cpp
    src/wad/wad.cpp
)

add_subdirectory(thirdparty)

include_directories("${CMAKE_SOURCE_DIR}/include/qformats")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glm")

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARY}
    glm
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")