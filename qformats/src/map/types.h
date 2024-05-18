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

    struct QPointEntity : QEntity
    {
        glm::vec3 origin;
    };

    struct MapFileFace
    {
        MapFileFace() = default;
        std::array<glm::dvec3, 3> planePoints;
        glm::dvec3 planeNormal;
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
        int textureID;
    };

    // GEOMETRY
    struct Vertex
    {
        glm::dvec3 point;
        glm::dvec3 normal;
        glm::vec2 uv;
        glm::vec4 tangent;

        inline bool checkLegalVertext(const std::vector<MapFileFace> &faces)
        {
            for (auto f : faces)
            {
                auto proj = glm::dot(f.planeNormal, point);
                if (proj > f.planeDist && abs(f.planeDist - proj) > 0.0008)
                    return false;
            }
            return true;
        }

        inline bool inList(const std::vector<Vertex> &list)
        {
            for (auto const &v : list)
            {
                if (v.point == point)
                    return true;
            }
            return false;
        }
    };

    class Plane
    {
    public:
        glm::dvec3 n; // Plane normal
        double d;     // D

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

        Plane(const glm::dvec3 n, const double d)
        {
            this->n = n;
            this->d = d;
        }

        Plane(const glm::dvec3 &a, const glm::dvec3 &b, const glm::dvec3 &c)
        {
            this->PointsToPlane(a, b, c);
        }

        void PointsToPlane(const glm::dvec3 &a, const glm::dvec3 &b, const glm::dvec3 &c)
        {
            n = glm::cross((c - b), (a - b));
            n = glm::normalize(n);
            d = glm::dot(-n, a);
        }

        double DistanceToPlane(const glm::dvec3 &v)
        {
            return (glm::dot(n, v) + d);
        }

        eCP ClassifyPoint(const glm::dvec3 &v)
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

        bool GetIntersection(const Plane &a, const Plane &b, glm::dvec3 &v)
        {
            double denom = glm::dot(n, glm::cross(a.n, b.n));
            if (fabs(denom) < epsilon)
                return false;

            v = ((glm::cross(a.n, b.n)) * -d - (glm::cross(b.n, n)) * a.d - (glm::cross(n, a.n)) * b.d) / denom;
            return true;
        }

        bool GetIntersection(const glm::dvec3 &start, const glm::dvec3 &end, glm::dvec3 &intersection, double &percentage)
        {
            glm::dvec3 dir = end - start;
            dir = glm::normalize(dir);
            double denom = glm::dot(n, dir);

            if (fabs(denom) < epsilon)
            {
                return false;
            }

            double num = -DistanceToPlane(start);
            percentage = num / denom;
            intersection = start + (dir * percentage);
            percentage = percentage / (end - start).length();
            return true;
        }
    };

}
