#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include "types.h"

using glm::vec2, glm::vec3, glm::vec4, glm::dvec3;

namespace qformats::map
{
    const float CMP_EPSILON = 0.008;
    const auto UP_VEC = glm::dvec3{0, 0, 1};
    const auto RIGHT_VEC = glm::dvec3{0, 1, 0};
    const auto FORWARD_VEC = glm::dvec3{1, 0, 0};

    class QMath
    {
    public:
        static vec4 CalcTangent(bool isValve, const MapFileFace &face)
        {
            return isValve ? calcValveTangent(face) : calcStandardTangent(face);
        };
        static vec2 CalcUV(bool isValve, vec3 vertex, const MapFileFace &face, int texW, int texH)
        {
            return isValve ? calcValveUV(vertex, face, texW, texH) : calcStandardUV(vertex, face, texW, texH);
        };

        static vec4 calcStandardTangent(const MapFileFace &face);
        static vec4 calcValveTangent(const MapFileFace &face);
        static vec2 calcStandardUV(vec3 vertex, const MapFileFace &face, int texW, int texH);
        static vec2 calcValveUV(vec3 vertex, const MapFileFace &face, int texW, int texH);
    };
}