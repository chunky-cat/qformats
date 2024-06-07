#pragma once

#include "types.h"
#include "qmath.h"
#include <memory>

namespace qformats::map
{
    class Face;
    using FacePtr = std::shared_ptr<Face>;
    using FaceIter = std::vector<FacePtr>::const_iterator;

    class Face
    {
    public:
        enum eCF
        {
            FRONT = 0,
            BACK,
            ONPLANE,
            SPANNING,
        };

    public:
        Face(){};
        Face::eCF Classify(const Face *other);
        FacePtr ClipToList(FaceIter &other, const FaceIter &end);

        void UpdateAB();
        bool IsOnSamePlane(const Face *other);
        bool isBigger(const Face *other);
        void UpdateNormals();
        std::shared_ptr<Face> Copy();

        int TextureID() {return textureID;};

        std::vector<Vertex> vertices;
        std::vector<unsigned short> indices;
        fvec3 center,min,max;
        bool noDraw = false;
        bool operator==(const Face &arg_) const;

        friend std::ostream& operator<<(std::ostream& stream, const Face& p)
        {
            stream << "polygon: ";
            for (auto& v : p.vertices)
            {

                stream << v << "  ";
            }

            stream << p.planeNormal;

            return stream;
        }

    private:
        fvec4 CalcTangent()
        {
            return hasValveUV ? calcValveTangent() : calcStandardTangent();
        };
        fvec2 CalcUV(fvec3 vertex, int texW, int texH)
        {
            return hasValveUV ? calcValveUV(vertex, texW, texH) : calcStandardUV(vertex, texW, texH);
        };

        fvec4 calcStandardTangent();
        fvec4 calcValveTangent();
        fvec2 calcStandardUV(fvec3 vertex, int texW, int texH);
        fvec2 calcValveUV(fvec3 vertex, int texW, int texH);


        std::array<fvec3, 3> planePoints;
        fvec3 planeNormal;
        float planeDist;
        StandardUV standardUv;
        ValveUV valveUV;
        double rotation;
        double scaleX;
        double scaleY;
        bool hasValveUV;
        int textureID;

        friend class Brush;
        friend class QMapFile;
        friend class QMap;
    };
}