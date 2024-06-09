#pragma once

#include "types.h"
#include "face.h"
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
        void buildGeometry(const std::map<int, bool> &excludedTextureIDs);

        std::vector<FacePtr> faces;

    private:
        fvec3 min;
        fvec3 max;

        void generatePolygons(const std::map<int, bool> &excludedTextureIDs);
        void windFaceVertices();
        std::vector<FacePtr> clipToBrush(const Brush& other);
        void indexFaceVertices();
        void calculateAABB();
        Vertex mergeDuplicate(int from, Vertex &v);
        boolRet<Vertex> intersectPlanes(const FacePtr &a, const FacePtr &b, const FacePtr &c);
        static bool isLegalVertex(const Vertex & v, const std::vector<FacePtr> &faces);

        FacePtr clipToList(FaceIter first, const FaceIter &firstEnd, FaceIter second, const FaceIter &secondEnd);

        friend class QMap;
        friend class QMapFile;
        friend class SolidEntity;
    };
}
