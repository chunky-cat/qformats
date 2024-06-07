#include "qformats/map/brush.h"
#include <qformats/map/qmath.h>
#include <iostream>

namespace qformats::map
{
const auto fv3zero = Vertex();

void
Brush::buildGeometry(const std::map<int, bool> &excludedTextureIDs)
{
    generatePolygons(excludedTextureIDs);
    windFaceVertices();
    indexFaceVertices();
    calculateAABB();
}

boolRet<Vertex>
Brush::intersectPlanes(const FacePtr &a, const FacePtr &b, const FacePtr &c)
{
    fvec3 n0 = a->planeNormal;
    fvec3 n1 = b->planeNormal;
    fvec3 n2 = c->planeNormal;

    float denom = dot(cross(n0, n1), n2);
    if (denom < CMP_EPSILON)
        return boolRet<Vertex>(false, fv3zero);

    Vertex v = {};
    v.point = (cross(n1, n2) * a->planeDist + cross(n2, n0) * b->planeDist + cross(n0, n1) * c->planeDist) / denom;

    return boolRet<Vertex>(true, v);
}

Vertex
Brush::mergeDuplicate(int from, Vertex &v)
{
    for (int n = 0; n <= from; n++)
    {
        auto otherPoly = faces[n];
        for (int i = 0; i < otherPoly->vertices.size(); i++)
        {
            if (dist3(otherPoly->vertices[i].point, v.point) < CMP_EPSILON)
            {
                return otherPoly->vertices[i];
            }
        }
    }
    return v;
}

void
Brush::indexFaceVertices()
{

    for (auto &f : faces)
    {
        if (f->vertices.size() < 3)
            continue;

        f->indices.resize((f->vertices.size() - 2) * 3);
        for (int i = 0; i < f->vertices.size() - 2; i++)
        {
            f->indices.push_back(0);
            f->indices.push_back(i + 1);
            f->indices.push_back(i + 2);
        }
        f->UpdateNormals();
    }
}

void
Brush::windFaceVertices()
{

    for (auto &f : faces)
    {
        if (f->vertices.size() < 3)
            continue;

        auto windFaceBasis = normalize(f->vertices[1].point - f->vertices[0].point);
        auto windFaceCenter = fvec3();
        auto windFaceNormal = normalize(f->planeNormal);

        for (auto v : f->vertices)
        {
            windFaceCenter += v.point;
        }
        windFaceCenter /= (float) f->vertices.size();

        std::stable_sort(f->vertices.begin(), f->vertices.end(), [&](Vertex l, Vertex r)
        {

            fvec3 u = normalize(windFaceBasis);
            fvec3 v = normalize(cross(u, windFaceNormal));

            fvec3 loc_a = l.point - windFaceCenter;
            float a_pu = dot(loc_a, u);
            float a_pv = dot(loc_a, v);

            fvec3 loc_b = r.point - windFaceCenter;
            float b_pu = dot(loc_b, u);
            float b_pv = dot(loc_b, v);

            float a_angle = atan2(a_pv, a_pu);
            float b_angle = atan2(b_pv, b_pu);

            if (a_angle == b_angle)
            {
                return 0;
            }
            return a_angle > b_angle ? 0 : 1;
        });
    }
}

FacePtr
Brush::clipToList(FaceIter first, const FaceIter &firstEnd, FaceIter second, const FaceIter &secondEnd)
{
    if (second->get() == nullptr)
    {
        std::cout << "second end" << std::endl;
        return nullptr;
    }
    auto ecp = second->get()->Classify(first->get());
    switch (ecp)
    {
        case Face::FRONT: // poligon is outside of brush
        {
            return *first;
        }
        case Face::BACK:
        {
            auto secNext = ++second;
            if (second == secondEnd || secNext->get() == nullptr)
            {
                return nullptr; // polygon is inside of brush
            }
            return clipToList(first, firstEnd, secNext, secondEnd);
        }
        case Face::ONPLANE:
        {
            float angle = dot(second->get()->planeNormal, first->get()->planeNormal) - 1;
            if (angle < CMP_EPSILON && angle > -CMP_EPSILON)
            {
                std::cout << "onplane" << std::endl;
                return *first;
            }
            auto secNext = ++second;
            if (second == secondEnd || secNext->get() == nullptr)
            {
                return nullptr; // polygon is inside of brush
            }

            return clipToList(first, firstEnd, secNext, secondEnd);
        }
        case Face::SPANNING:std::cout << "spanning" << std::endl;
            break;
    }
    return *first;
}

void
Brush::clipToBrush(const Brush &other)
{
    auto otherFaces_iter = other.faces.begin();
    auto faces_iter = faces.begin();
    std::vector<FacePtr> clippedFaces;

    while (faces_iter != faces.cend())
    {
        auto clippedPoly = clipToList(faces_iter, faces.cend(), otherFaces_iter, other.faces.cend());
        if (clippedPoly != nullptr)
        {
            clippedFaces.push_back(clippedPoly);
        }
        ++faces_iter;
    }

    faces = clippedFaces;
}

fvec3
GetUnitNormal(const fvec2 p1, const fvec2 p2, const float s)
{
    const fvec2 p3 = p1 + ((p2 - p1) * s);

    const float m = (p3[1] - p1[1]) / (p3[0] - p1[0]);
    const float c = p1[1] - m * p1[0];
    const float y = (m * p1[0]) + c;

    const fvec2 tangent = normalize(fvec2(p1[0], p2[1]));
    fvec3 normal = fvec3(-tangent[1], 0, tangent[0]);

    return normalize(normal);
}

void
Brush::generatePolygons(const std::map<int, bool> &excludedTextureIDs)
{
    float phongAngle = 89.0;

    for (int i = 0; i < faces.size(); i++)
    {
        for (int j = 0; j < faces.size(); j++)
            for (int k = 0; k < faces.size(); k++)
            {
                if (i == j && i == k && j == k)
                    continue;

                if (excludedTextureIDs.count(faces[k]->textureID))
                    faces[k]->noDraw = true;

                auto res = intersectPlanes(faces[i], faces[j], faces[k]);
                if (!res.first || !isLegalVertex(res.second, faces))
                    continue;

                res.second = mergeDuplicate(i, res.second);

                auto v = res.second;
                v.normal = faces[i]->planeNormal;
                v.normal = normalize(v.normal);
                v.tangent = faces[k]->CalcTangent();

                if (v.inList(faces[k]->vertices))
                    continue;
                faces[k]->vertices.push_back(v);
            }
        faces[i]->UpdateAB();
    }
}

bool
Brush::isLegalVertex(const Vertex &v, const std::vector<FacePtr> &faces)
{
    for (const auto &f : faces)
    {
        auto proj = tue::math::dot(f->planeNormal, v.point);
        if (proj > f->planeDist && abs(f->planeDist - proj) > 0.0008)
        {
            return false;
        }
    }
    return true;
}

bool
Brush::DoesIntersect(const Brush &other)
{
    if ((min[0] > other.max[0]) || (other.min[0] > max[0]))
        return false;

    if ((min[1] > other.max[1]) || (other.min[1] > max[1]))
        return false;

    if ((min[2] > other.max[2]) || (other.min[2] > max[2]))
        return false;

    return true;
}

void
Brush::calculateAABB()
{
    if (faces[0]->vertices.size() == 0)
        return;

    min = faces[0]->vertices[0].point;
    max = faces[0]->vertices[0].point;

    for (const auto &face : faces)
        for (const auto &vert : face->vertices)
        {
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
}
