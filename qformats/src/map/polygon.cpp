#include "polygon.h"

namespace qformats::map
{
    using polyPair = std::pair<std::shared_ptr<Polygon>, std::shared_ptr<Polygon>>;
    using polyPtr = std::shared_ptr<Polygon>;

    polyPair Polygon::SplitPoly(Polygon *other)
    {
        Plane::eCP *pCP = new Plane::eCP[other->vertices.size()];
        for (int i = 0; i < other->vertices.size(); i++)
            pCP[i] = plane.ClassifyPoint(other->vertices[i].point);

        auto front = std::make_shared<Polygon>();
        auto back = std::make_shared<Polygon>();

        front->faceRef = other->faceRef;
        back->faceRef = other->faceRef;
        front->plane = other->plane;
        back->plane = other->plane;

        for (int i = 0; i < other->vertices.size(); i++)
        {
            switch (pCP[i])
            {
            case Plane::eCP::FRONT:
            {
                front->vertices.push_back(other->vertices[i]);
            }
            break;

            case Plane::eCP::BACK:
            {
                back->vertices.push_back(other->vertices[i]);
            }
            break;

            case Plane::eCP::ONPLANE:
            {
                front->vertices.push_back(other->vertices[i]);
                back->vertices.push_back(other->vertices[i]);
            }
            break;
            }

            //
            // Check if edges should be split
            //
            int iNext = i + 1;
            bool bIgnore = false;

            if (i == (other->vertices.size() - 1))
            {
                iNext = 0;
            }

            if ((pCP[i] == Plane::eCP::ONPLANE) && (pCP[iNext] != Plane::eCP::ONPLANE))
            {
                bIgnore = true;
            }
            else if ((pCP[iNext] == Plane::eCP::ONPLANE) && (pCP[i] != Plane::eCP::ONPLANE))
            {
                bIgnore = true;
            }

            if ((!bIgnore) && (pCP[i] != pCP[iNext]))
            {
                Vertex v; // New vertex created by splitting
                double p; // Percentage between the two points

                plane.GetIntersection(other->vertices[i].point, other->vertices[iNext].point, v.point, p);

                v.uv[0] = other->vertices[iNext].uv[0] - other->vertices[i].uv[0];
                v.uv[1] = other->vertices[iNext].uv[1] - other->vertices[i].uv[1];

                v.uv[0] = other->vertices[i].uv[0] + (p * v.uv[0]);
                v.uv[1] = other->vertices[i].uv[1] + (p * v.uv[1]);

                front->vertices.push_back(v);
                back->vertices.push_back(v);
            }
        }

        front->CalculatePlane();
        back->CalculatePlane();

        return polyPair(front, back);
    }

    polyPtr Polygon::Copy()
    {
        auto newp = std::make_shared<Polygon>();
        newp->vertices = vertices;
        newp->indices = indices;
        newp->faceRef = faceRef;
        newp->plane = plane;
        return newp;
    }

    const bool Polygon::operator==(const Polygon &other) const
    {

        if (vertices.size() != other.vertices.size() || plane.d != other.plane.d || plane.n != other.plane.n)
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
            double dist = glm::dot(plane.n, other->vertices[i].point) + plane.d;
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

    bool Polygon::CalculatePlane()
    {
        glm::dvec3 centerOfMass;
        double magnitude;
        int i, j;

        if (vertices.size() < 3)
        {
            return false;
        }

        plane.n.x = 0.0f;
        plane.n.y = 0.0f;
        plane.n.z = 0.0f;
        centerOfMass.x = 0.0f;
        centerOfMass.y = 0.0f;
        centerOfMass.z = 0.0f;

        for (i = 0; i < vertices.size(); i++)
        {
            j = i + 1;

            if (j >= vertices.size())
            {
                j = 0;
            }

            plane.n.x += (vertices[i].point.y - vertices[j].point.y) * (vertices[i].point.z + vertices[j].point.z);
            plane.n.y += (vertices[i].point.z - vertices[j].point.z) * (vertices[i].point.x + vertices[j].point.x);
            plane.n.z += (vertices[i].point.x - vertices[j].point.x) * (vertices[i].point.y + vertices[j].point.y);

            centerOfMass.x += vertices[i].point.x;
            centerOfMass.y += vertices[i].point.y;
            centerOfMass.z += vertices[i].point.z;
        }

        if ((fabs(plane.n.x) < epsilon) && (fabs(plane.n.y) < epsilon) &&
            (fabs(plane.n.z) < epsilon))
        {
            return false;
        }

        magnitude = sqrt(plane.n.x * plane.n.x + plane.n.y * plane.n.y + plane.n.z * plane.n.z);

        if (magnitude < epsilon)
        {
            return false;
        }

        plane.n.x /= magnitude;
        plane.n.y /= magnitude;
        plane.n.z /= magnitude;

        centerOfMass.x /= (double)vertices.size();
        centerOfMass.y /= (double)vertices.size();
        centerOfMass.z /= (double)vertices.size();

        plane.d = -glm::dot(centerOfMass, plane.n);

        return true;
    }
}