//
// Created by tgoeh on 10.11.2023.
//
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "types.h"
#include "map_file.h"

namespace qformats::map
{
    class QMap
    {
    public:
        static QMap *FromFIle(const std::string &filename);
        QMap() = default;
        ~QMap() = default;
        QMapFile *MapData() { return map_file; };
        std::vector<QBrushEntity *> &BrushEntities() { return map_file->brushEntities; };

        void generatePolygons();

    public:
        // std::vector<EntityGeometry *> brushEntities;
        std::vector<std::string> textures;

    private:
        void generatePolygonsForBrush(QBrush *brush);
        void windFaceVertices();
        void windFaceVertices2();
        std::pair<bool, glm::vec3> intersectPlanes(QFace &a, QFace &b, QFace &c);
        bool checkLegalVertext(glm::vec3 vert, std::vector<QFace> faces);
        QMapFile *map_file;
    };
}
