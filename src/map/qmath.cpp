#include "qmath.h"

namespace qformats::map
{

    fvec4 QMath::calcStandardTangent(const MapFileFace &face)
    {
        float du = dot(face.planeNormal, UP_VEC);
        float dr = dot(face.planeNormal, RIGHT_VEC);
        float df = dot(face.planeNormal, FORWARD_VEC);
        float dua = abs(du);
        float dra = abs(dr);
        float dfa = abs(df);

        fvec3 uAxis{0};
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
        uAxis = tue::transform::rotation_vec(uAxis, (float)((-face.rotation * vSign) * (180.0 / 3.141592653589793238463)));
        return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
    }

    fvec4 QMath::calcValveTangent(const MapFileFace &face)
    {
        fvec3 uAxis = normalize(face.valveUV.u.xyz());
        fvec3 vAxis = normalize(face.valveUV.v.xyz());
        float vSign = copysignf(1.0, dot(cross((fvec3)face.planeNormal, uAxis), vAxis));
        return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
    }

    fvec2 QMath::calcStandardUV(fvec3 vertex, const MapFileFace &face, int texW, int texH)
    {
        fvec2 uvOut{0};

        float du = abs(dot(face.planeNormal, UP_VEC));
        float dr = abs(dot(face.planeNormal, RIGHT_VEC));
        float df = abs(dot(face.planeNormal, FORWARD_VEC));

        if (du >= dr && du >= df)
            uvOut = fvec2(vertex[0], -vertex[1]);
        else if (dr >= du && dr >= df)
            uvOut = fvec2(vertex[0], -vertex[2]);
        else if (df >= du && df >= dr)
            uvOut = fvec2(vertex[1], -vertex[2]);

        float angle = face.rotation * (M_PI / 180);
        uvOut = fvec2(
            uvOut[0] * cos(angle) - uvOut[1] * sin(angle),
            uvOut[0] * sin(angle) + uvOut[1] * cos(angle));

        uvOut[0] /= texW;
        uvOut[1] /= texH;

        uvOut[0] /= face.scaleX;
        uvOut[1] /= face.scaleY;

        uvOut[0] += face.standardUv.u / texW;
        uvOut[1] += face.standardUv.v / texH;
        return uvOut;
    }

    fvec2 QMath::calcValveUV(fvec3 vertex, const MapFileFace &face, int texW, int texH)
    {
        fvec2 uvOut{0};
        fvec3 uAxis = face.valveUV.u.xyz();
        fvec3 vAxis = face.valveUV.v.xyz();
        float uShift = face.valveUV.u[3];
        float vShift = face.valveUV.v[3];

        uvOut[0] = dot(uAxis, vertex);
        uvOut[1] = dot(vAxis, vertex);

        uvOut[0] /= texW;
        uvOut[1] /= texH;

        uvOut[0] /= face.scaleX;
        uvOut[1] /= face.scaleY;

        uvOut[0] += uShift / texW;
        uvOut[1] += vShift / texH;

        return uvOut;
    }
}