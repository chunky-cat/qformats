#include "qformats/map/polygon.h"
#include <iostream>

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

    void Polygon::RecalcNormals()
    {

        for (int i = 0; i < indices.size(); i += 3)
        {
            // get the three vertices that make the faces
            const auto &p1 = vertices[indices[i + 0]].point;
            const auto &p2 = vertices[indices[i + 1]].point;
            const auto &p3 = vertices[indices[i + 2]].point;

            auto v1 = p2 - p1;
            auto v2 = p3 - p1;
            auto normal = normalize(cross(v1, v2));

            vertices[indices[i + 0]].normal = normal;
            vertices[indices[i + 1]].normal = normal;
            vertices[indices[i + 2]].normal = normal;
        }
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
            double dist = dot(faceRef.planeNormal, other->vertices[i].point);
            //proj > f.planeDist && abs(f.planeDist - proj) > 0.0008
            if(dist > faceRef.planeDist && abs(faceRef.planeDist - dist) > 0.0008)
            //if (dist > 0.001)
            {
                if (bBack)
                {
                    return eCP::SPANNING;
                }

                bFront = true;
            }
            else if (dist < faceRef.planeDist&& abs(faceRef.planeDist + dist) < -0.0008)
            //else if (dist < -0.001)
            {
                if (bFront)
                {
                    return eCP::SPANNING;
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

    PolygonPtr qformats::map::Polygon::ClipToList(PolygonIter& other, const PolygonIter& end)
    {
        auto ecp = ClassifyPoly(other->get());
        std::cout << "ecp: " << ecp << std::endl;
        switch (ecp)
        {
        case FRONT: // poligon is outside of brush
            return *other;
        case BACK:
            if (other == end)
            {
                return nullptr; // polygon is inside of brush
            }
            return ClipToList(++other, end);
        }
    }
}