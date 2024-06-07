#include "qformats/map/face.h"
#include <iostream>

namespace qformats::map
{
    using polyPair = std::pair<std::shared_ptr<Face>, std::shared_ptr<Face>>;
    using polyPtr = std::shared_ptr<Face>;

    polyPtr Face::Copy()
    {
        auto newp = std::make_shared<Face>();
        newp->vertices = vertices;
        newp->indices = indices;
        return newp;
    }

    void Face::UpdateNormals()
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

    Face::eCF Face::Classify(const Face *other)
    {
        bool bFront = false, bBack = false;
        for (int i = 0; i < (int)other->vertices.size(); i++)
        {
            double dist = dot(planeNormal, other->vertices[i].point);
            if(dist > planeDist && abs(planeDist - dist) > 0.0008)
            {
                if (bBack)
                {
                    return eCF::SPANNING;
                }

                bFront = true;
            }
            else if (dist < planeDist && planeDist + dist < -0.0008)
            {
                if (bFront)
                {
                    return eCF::SPANNING;
                }

                bBack = true;
            }
        }

        if (bFront)
        {
            return eCF::FRONT;
        }
        else if (bBack)
        {
            return eCF::BACK;
        }

        return eCF::ONPLANE;
    }

    FacePtr qformats::map::Face::ClipToList(FaceIter& other, const FaceIter& end)
    {
        auto ecp = Classify(other->get());
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

    void Face::UpdateAB() {
        if (vertices.empty())
            return;

        min = vertices[0].point;
        max = vertices[0].point;

        for (const auto &vert: vertices) {
            if (vert.point[0] < min[0])
                min[0] = vert.point[0];

            if (vert.point[1] < min[1])
                min[1] = vert.point[1];

            if (vert.point[2] < min[2])
                min[2] = vert.point[2];

            if (vert.point[0] > max[0])
                max[0] = vert.point[0];

            if (vert.point[1] > max[1])
                max[1] = vert.point[1];

            if (vert.point[2] > max[2])
                max[2] = vert.point[2];
        }
    }

    bool Face::IsOnSamePlane(const Face *other)
    {
        for (size_t i = 0; i < vertices.size(); i++)
        {
            if (other->vertices.size() <= i) break;
            auto v = vertices[i].point*planeNormal - other->vertices[i].point *planeNormal;
            if (tue::math::length(v) != 0) {
                return false;
            }
        }
        return true;
    }

    bool Face::operator==(const Face &other) const
    {
        if (vertices.size() != other.vertices.size() || planeDist != other.planeDist || planeNormal != other.planeNormal)
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

        if (textureID == other.textureID)
            return true;

        return true;
    }

    bool Face::isBigger(const Face *other)
    {
        if (max[0] > other->max[0] || max[1] > other->max[1] || max[2] > other->max[2]) return true;
        if (min[0] < other->min[0] || min[1] < other->min[1] || min[2] < other->min[2]) return true;
        return false;
    }

    fvec4 Face::calcStandardTangent()
    {
        float du = dot(planeNormal, UP_VEC);
        float dr = dot(planeNormal, RIGHT_VEC);
        float df = dot(planeNormal, FORWARD_VEC);
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

        vSign *= copysignf(1.0, scaleY);
        uAxis = tue::transform::rotation_vec(uAxis, (float)((-rotation * vSign) * (180.0 / 3.141592653589793238463)));
        return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
    }

    fvec4 Face::calcValveTangent()
    {
        fvec3 uAxis = normalize(valveUV.u.xyz());
        fvec3 vAxis = normalize(valveUV.v.xyz());
        float vSign = copysignf(1.0, dot(cross((fvec3)planeNormal, uAxis), vAxis));
        return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
    }

    fvec2 Face::calcStandardUV(fvec3 vertex, int texW, int texH)
    {
        fvec2 uvOut{0};

        float du = abs(dot(planeNormal, UP_VEC));
        float dr = abs(dot(planeNormal, RIGHT_VEC));
        float df = abs(dot(planeNormal, FORWARD_VEC));

        if (du >= dr && du >= df)
            uvOut = fvec2(vertex[0], -vertex[1]);
        else if (dr >= du && dr >= df)
            uvOut = fvec2(vertex[0], -vertex[2]);
        else if (df >= du && df >= dr)
            uvOut = fvec2(vertex[1], -vertex[2]);

        float angle = rotation * (M_PI / 180);
        uvOut = fvec2(
                uvOut[0] * cos(angle) - uvOut[1] * sin(angle),
                uvOut[0] * sin(angle) + uvOut[1] * cos(angle));

        uvOut[0] /= texW;
        uvOut[1] /= texH;

        uvOut[0] /= scaleX;
        uvOut[1] /= scaleY;

        uvOut[0] += standardUv.u / texW;
        uvOut[1] += standardUv.v / texH;
        return uvOut;
    }

    fvec2 Face::calcValveUV(fvec3 vertex, int texW, int texH)
    {
        fvec2 uvOut{0};
        fvec3 uAxis = valveUV.u.xyz();
        fvec3 vAxis = valveUV.v.xyz();
        float uShift = valveUV.u[3];
        float vShift = valveUV.v[3];

        uvOut[0] = dot(uAxis, vertex);
        uvOut[1] = dot(vAxis, vertex);

        uvOut[0] /= texW;
        uvOut[1] /= texH;

        uvOut[0] /= scaleX;
        uvOut[1] /= scaleY;

        uvOut[0] += uShift / texW;
        uvOut[1] += vShift / texH;

        return uvOut;
    }
}