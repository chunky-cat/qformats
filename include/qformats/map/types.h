#pragma once

#include <tue/transform.hpp>

#include <string>
#include <vector>
#include <array>
#include <map>
#include <tue/vec.hpp>

using tue::fvec2;
using tue::fvec3;
using tue::fvec4;

using tue::math::cross;
using tue::math::dot;
using tue::math::normalize;

inline float dist3(const fvec3 &a, const fvec3 &b)
{
    fvec3 diff = b - a;
    return sqrtf(dot(diff, diff));
};

namespace qformats::map
{
    const double epsilon = 1e-5; // Used to compensate for floating point inaccuracy.
    const double scale = 128;    // Scale
    // MAP FILE

    struct QEntity
    {
        std::string classname;
        float angle;
        std::map<std::string, std::string> attributes;
    };

    struct QPointEntity : QEntity
    {
        fvec3 origin;
    };

    struct MapFileFace
    {
        MapFileFace() = default;
        std::array<fvec3, 3> planePoints;
        fvec3 planeNormal;
        float planeDist;
        struct StandardUV
        {
            float u;
            float v;
        };
        StandardUV standardUv;
        struct ValveUV
        {
            fvec4 u;
            fvec4 v;
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
        fvec3 point;
        fvec3 normal;
        fvec2 uv;
        fvec4 tangent;

        inline bool checkLegalVertext(const std::vector<MapFileFace> &faces)
        {
            for (auto f : faces)
            {

                auto proj = tue::math::dot(f.planeNormal, point);
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
}
