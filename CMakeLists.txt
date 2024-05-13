cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

project(
    quaken
    VERSION 1.0
    LANGUAGES CXX
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMACOSX)
endif()

find_package(OpenGL REQUIRED) # error

add_subdirectory(thirdparty)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/map/parser.cpp
    src/map/map.cpp
    src/map/types.h
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARY}
    glm
)

add_custom_target(
    run-qformat-sample
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
)


#set(raylib_VERBOSE 0)
target_link_libraries(${PROJECT_NAME} raylib)

add_dependencies(run-qformat-sample ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
