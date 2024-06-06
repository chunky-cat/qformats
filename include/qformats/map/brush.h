#pragma once

#include "types.h"
#include "polygon.h"
#include <vector>
#include <memory>

namespace qformats::map
{
    template <class T>
    using boolRet = std::pair<bool, T>;
    class Brush
    {
    public:
        Brush(){};
        bool DoesIntersect(const Brush& other);
        std::vector<PolygonPtr> polygons;

    private:
        std::vector<MapFileFace> faces;
        bool hasValveUV;

        fvec3 min;
        fvec3 max;

        void buildGeometry(const std::map<int, bool> &excludedTextureIDs);
        void generatePolygons(const std::map<int, bool> &excludedTextureIDs);
        void windFaceVertices();
        void clipToBrush(const Brush& other);
        void indexFaceVertices();
        void calculateAABB();
        fvec4 calcStandardTangent(const MapFileFace &face);
        fvec4 calcValveTangent(const MapFileFace &face);
        fvec2 calcStandardUV(fvec3 vertex, const MapFileFace &face, int texW, int texH);
        fvec2 calcValveUV(fvec3 vertex, const MapFileFace &face, int texW, int texH);
        Vertex mergeDuplicate(int from, Vertex &v);
        boolRet<Vertex> intersectPlanes(const MapFileFace &a, const MapFileFace &b, const MapFileFace &c);

        friend class QMap;
        friend class QMapFile;
    };
}
