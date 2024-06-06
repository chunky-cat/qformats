#pragma once

#include "types.h"
#include "qmath.h"
#include <memory>

namespace qformats::map
{
    class Polygon;
    using PolygonPtr = std::shared_ptr<Polygon>;
    using PolygonIter = std::vector<PolygonPtr>::const_iterator;

    class Polygon
    {
    public:
        enum eCP
        {
            FRONT = 0,
            BACK,
            ONPLANE,
            SPANNING,
        };

    public:
        Polygon(){};
        Polygon::eCP ClassifyPoly(const Polygon *other);
        PolygonPtr ClipToList(PolygonIter &other, const PolygonIter &end);
        void RecalcNormals();
        std::shared_ptr<Polygon> Copy();

        MapFileFace faceRef;
        std::vector<Vertex> vertices;
        std::vector<unsigned short> indices;
        fvec3 center;
        bool noDraw = false;
        const bool operator==(const Polygon &arg_) const;

    private:
        bool last = false;
    };

    static std::ostream& operator<<(std::ostream& stream, const Polygon& p)
    {
        stream << "polygon: ";
        for (auto& v : p.vertices)
        {

            stream << v << "  ";
        }

        stream << p.faceRef.planeNormal;

        return stream;
    }

}