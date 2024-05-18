#pragma once

#include "types.h"
#include <memory>

namespace qformats::map
{

    class Polygon
    {
    public:
        enum eCP
        {
            FRONT = 0,
            BACK,
            ONPLANE,
            SPLIT,
        };

    public:
        Polygon(){};
        Polygon::eCP ClassifyPoly(const Polygon *other);
        bool CalculatePlane();
        std::pair<std::shared_ptr<Polygon>, std::shared_ptr<Polygon>> SplitPoly(Polygon *other);
        std::shared_ptr<Polygon> Copy();

        MapFileFace faceRef;
        std::vector<Vertex> vertices;
        std::vector<ushort> indices;
        glm::vec3 center;
        Plane plane;
        const bool operator==(const Polygon &arg_) const;

    private:
        bool last = false;
    };
}