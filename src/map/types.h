#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

namespace qformats::map
{
    const double epsilon = 1e-5; // Used to compensate for floating point inaccuracy.
    const double scale = 128;    // Scale
    // MAP FILE

    struct QEntity
    {
        std::string classname;
        std::map<std::string, std::string> attributes;
    };

    ////////////////////////////////////////////////////////////////////
    // Name:		Plane
    // Description:	Plane class.  "double" is used for greater accuracy.
    //				Follows N dot P + D = 0 equation.
    ////////////////////////////////////////////////////////////////////
    class Plane
    {
    public:
        glm::vec3 n; // Plane normal
        double d;    // D

        enum eCP
        {
            FRONT = 0,
            BACK,
            ONPLANE
        };

        Plane()
        {
            d = 0;
        }

        Plane(const glm::vec3 n, const double d)
        {
            this->n = n;
            this->d = d;
        }

        Plane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
        {
            n = glm::cross((c - b), (a - b));
            n = glm::normalize(n);

            d = -glm::dot(n, a);
        }

        void PointsToPlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
        {
            n = glm::cross((c - b), (a - b));
            n = glm::normalize(n);

            d = -glm::dot(n, a);
        }

        double DistanceToPlane(const glm::vec3 &v)
        {
            return (glm::dot(n, v) + d);
        }

        eCP ClassifyPoint(const glm::vec3 &v)
        {
            double Distance = DistanceToPlane(v);

            if (Distance > epsilon)
            {
                return eCP::FRONT;
            }
            else if (Distance < -epsilon)
            {
                return eCP::BACK;
            }

            return eCP::ONPLANE;
        }
    };

    struct QFace
    {
        QFace() = default;
        std::array<glm::vec3, 3> planePoints;
        glm::vec3 planeNormal;
        float planeDist;
        struct StandardUV
        {
            float u;
            float v;
        };
        StandardUV standardUv;
        struct ValveUV
        {
            glm::dvec4 u;
            glm::dvec4 v;
        };
        ValveUV valveUV;
        double rotation;
        double scaleX;
        double scaleY;
        std::string texture;
        Plane plane;
    };

    struct Polygon
    {
        QFace face;
        std::vector<glm::vec3> vertices;
        std::vector<int> indices;
        glm::vec3 center;
        Plane plane;
    };

    struct QBrush
    {
        std::vector<QFace> faces;
        std::vector<Polygon> polys;
        int vertCount = 0;
        ushort indiceCount = 0;
    };

    struct QBrushEntity : QEntity
    {
        std::vector<QBrush *> brushes;
        std::string tbType;
        std::string tbName;
        bool hasPhongShading;
    };

    struct QPointEntity : QEntity
    {
        glm::vec3 origin;
    };

    // GEOMETRY

    struct FaceVertex
    {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::dvec2 uv;
        glm::dvec4 tangent;
    };
}
