#include "qmath.h"
// TODO: remove dependency
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

namespace qformats::map
{

    glm::vec4 QMath::calcStandardTangent(const MapFileFace &face)
    {
        float du = glm::dot(face.planeNormal, UP_VEC);
        float dr = glm::dot(face.planeNormal, RIGHT_VEC);
        float df = glm::dot(face.planeNormal, FORWARD_VEC);
        float dua = abs(du);
        float dra = abs(dr);
        float dfa = abs(df);

        glm::vec3 uAxis{0};
        float vSign = 0.0f;

        if (dua >= dra && dua >= dfa)
        {
            uAxis = FORWARD_VEC;
            vSign = copysignf(1.0, du);
        }
        else if (dra >= dua && dra >= dfa)
        {
            uAxis = FORWARD_VEC;
            vSign = -copysignf(1.0, dr);
        }
        else if (dfa >= dua && dfa >= dra)
        {
            uAxis = RIGHT_VEC;
            vSign = copysignf(1.0, df);
        }

        vSign *= copysignf(1.0, face.scaleY);
        uAxis = glm::rotate(uAxis, (float)glm::degrees(-face.rotation * vSign), (glm::vec3)face.planeNormal);
        return glm::vec4(uAxis.x, uAxis.y, uAxis.z, vSign);
    }

    glm::vec4 QMath::calcValveTangent(const MapFileFace &face)
    {
        glm::vec3 uAxis = glm::normalize(face.valveUV.u);
        glm::vec3 vAxis = glm::normalize(face.valveUV.v);
        float vSign = copysignf(1.0, glm::dot(glm::cross((glm::vec3)face.planeNormal, uAxis), vAxis));
        return glm::vec4(uAxis.x, uAxis.y, uAxis.z, vSign);
    }

    glm::vec2 QMath::calcStandardUV(glm::vec3 vertex, const MapFileFace &face, int texW, int texH)
    {
        glm::vec2 uvOut{0};

        float du = abs(glm::dot(face.planeNormal, UP_VEC));
        float dr = abs(glm::dot(face.planeNormal, RIGHT_VEC));
        float df = abs(glm::dot(face.planeNormal, FORWARD_VEC));

        if (du >= dr && du >= df)
            uvOut = glm::vec2(vertex.x, -vertex.y);
        else if (dr >= du && dr >= df)
            uvOut = glm::vec2(vertex.x, -vertex.z);
        else if (df >= du && df >= dr)
            uvOut = glm::vec2(vertex.y, -vertex.z);

        float angle = glm::degrees(face.rotation);
        uvOut = glm::vec2(uvOut.x * cos(angle) - sin(angle), uvOut.x * sin(angle) + cos(angle));

        uvOut.x /= texW;
        uvOut.y /= texH;

        uvOut.x /= face.scaleX;
        uvOut.y /= face.scaleY;

        uvOut.x += face.standardUv.u / texW;
        uvOut.y += face.standardUv.v / texH;
        return uvOut;
    }

    glm::vec2 QMath::calcValveUV(glm::vec3 vertex, const MapFileFace &face, int texW, int texH)
    {
        glm::vec2 uvOut{0};
        glm::vec3 uAxis = face.valveUV.u;
        glm::vec3 vAxis = face.valveUV.v;
        float uShift = face.valveUV.u[3];
        float vShift = face.valveUV.v[3];

        uvOut.x = glm::dot(uAxis, vertex);
        uvOut.y = glm::dot(vAxis, vertex);

        uvOut.x /= texW;
        uvOut.y /= texH;

        uvOut.x /= face.scaleX;
        uvOut.y /= face.scaleY;

        uvOut.x += uShift / texW;
        uvOut.y += vShift / texH;

        return uvOut;
    }
}