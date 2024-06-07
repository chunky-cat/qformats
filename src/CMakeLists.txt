add_library(${PROJECT_NAME} STATIC
    common/textureman.cpp    
    map/map_file.cpp
    map/map.cpp
    map/brush.cpp
    map/qmath.cpp
        map/face.cpp
    wad/palette.cpp
    wad/wad.cpp
        map/entity_solid.cpp
        ../include/qformats/map/entities.h
)


target_include_directories(${PROJECT_NAME} PUBLIC "../include/")
