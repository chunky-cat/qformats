#include "../../include/qformats/map/polygon.h"

namespace qformats::map
{
    using polyPair = std::pair<std::shared_ptr<Polygon>, std::shared_ptr<Polygon>>;
    using polyPtr = std::shared_ptr<Polygon>;

    polyPtr Polygon::Copy()
    {
        auto newp = std::make_shared<Polygon>();
        newp->vertices = vertices;
        newp->indices = indices;
        newp->faceRef = faceRef;
        return newp;
    }

    const bool Polygon::operator==(const Polygon &other) const
    {

        if (vertices.size() != other.vertices.size() || faceRef.planeDist != other.faceRef.planeDist || faceRef.planeNormal != other.faceRef.planeNormal)
            return false;

        for (int i = 0; i < vertices.size(); i++)
        {
            if (vertices[i].point != other.vertices[i].point)
                return false;

            if (vertices[i].uv[0] != other.vertices[i].uv[0])
                return false;

            if (vertices[i].uv[1] != other.vertices[i].uv[1])
                return false;
        }

        if (faceRef.textureID == other.faceRef.textureID)
            return true;

        return true;
    }

    Polygon::eCP Polygon::ClassifyPoly(const Polygon *other)
    {
        bool bFront = false, bBack = false;
        for (int i = 0; i < (int)other->vertices.size(); i++)
        {
            double dist = glm::dot(faceRef.planeNormal, other->vertices[i].point) + faceRef.planeDist;
            if (dist > 0.001)
            {
                if (bBack)
                {
                    return eCP::SPLIT;
                }

                bFront = true;
            }
            else if (dist < -0.001)
            {
                if (bFront)
                {
                    return eCP::SPLIT;
                }

                bBack = true;
            }
        }

        if (bFront)
        {
            return eCP::FRONT;
        }
        else if (bBack)
        {
            return eCP::BACK;
        }

        return eCP::ONPLANE;
    }
}