add_library(${PROJECT_NAME} SHARED
    common/textureman.cpp    
    map/map_file.cpp
    map/map.cpp
    map/brush.cpp
    map/qmath.cpp
    map/polygon.cpp
    wad/palette.cpp
    wad/wad.cpp
)


target_include_directories(${PROJECT_NAME} PUBLIC "../include/")
target_include_directories(${PROJECT_NAME} PUBLIC "../thirdparty/glm")

target_link_libraries(${PROJECT_NAME}
    glm
)
