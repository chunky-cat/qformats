cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 20)

project(
    qformats
    VERSION 1.0.0
    LANGUAGES CXX
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMACOSX)
endif()

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

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
target_include_directories(${PROJECT_NAME} PUBLIC "thirdparty/glm")

target_link_libraries(${PROJECT_NAME}
    glm
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")