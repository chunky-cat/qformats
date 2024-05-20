#include "../../include/qformats/map/brush.h"
#include "qmath.h"

namespace qformats::map
{
    const auto fv3zero = Vertex();

    void Brush::buildGeometry()
    {
        generatePolygons();
        windFaceVertices();
        indexFaceVertices();
        calculateAABB();
    }

    boolRet<Vertex> Brush::intersectPlanes(const MapFileFace &a, const MapFileFace &b, const MapFileFace &c)
    {
        glm::vec3 n0 = a.planeNormal;
        glm::vec3 n1 = b.planeNormal;
        glm::vec3 n2 = c.planeNormal;

        float denom = glm::dot(glm::cross(n0, n1), n2);
        if (denom < CMP_EPSILON)
            return boolRet<Vertex>(false, fv3zero);

        Vertex v = {};
        v.point = (glm::cross(n1, n2) * a.planeDist + glm::cross(n2, n0) * b.planeDist + glm::cross(n0, n1) * c.planeDist) / denom;

        return boolRet<Vertex>(true, v);
    }

    Vertex Brush::mergeDuplicate(int from, Vertex &v)
    {
        for (int n = 0; n <= from; n++)
        {
            auto otherPoly = polygons[n];
            for (int i = 0; i < otherPoly->vertices.size(); i++)
            {
                if (glm::distance(otherPoly->vertices[i].point, v.point) < CMP_EPSILON)
                {
                    return otherPoly->vertices[i];
                }
            }
        }
        return v;
    }

    void Brush::indexFaceVertices()
    {

        for (auto &p : polygons)
        {
            if (p->vertices.size() < 3)
                continue;

            p->indices.resize((p->vertices.size() - 2) * 3);
            for (int i = 0; i < p->vertices.size() - 2; i++)
            {
                p->indices.push_back(0);
                p->indices.push_back(i + 1);
                p->indices.push_back(i + 2);
            }
        }
    }

    void Brush::windFaceVertices()
    {

        for (auto &p : polygons)
        {
            if (p->vertices.size() < 3)
                continue;

            auto windFaceBasis = glm::normalize(p->vertices[1].point - p->vertices[0].point);
            auto windFaceCenter = glm::dvec3();
            auto windFaceNormal = glm::normalize(p->faceRef.planeNormal);

            for (auto v : p->vertices)
            {
                windFaceCenter += v.point;
            }
            windFaceCenter /= (float)p->vertices.size();

            std::stable_sort(p->vertices.begin(), p->vertices.end(), [&](Vertex l, Vertex r)
                             {

                            glm::dvec3 u = glm::normalize(windFaceBasis);
                            glm::dvec3 v = glm::normalize(glm::cross(u, windFaceNormal));

                            glm::dvec3 loc_a = l.point - windFaceCenter;
                            float a_pu = glm::dot(loc_a, u);
                            float a_pv = glm::dot(loc_a, v);

                            glm::dvec3 loc_b = r.point - windFaceCenter;
                            float b_pu = glm::dot(loc_b, u);
                            float b_pv = glm::dot(loc_b, v);

                            float a_angle = atan2(a_pv, a_pu);
                            float b_angle = atan2(b_pv, b_pu);

                            if (a_angle == b_angle) {
                                return 0;
                            }
                            return a_angle > b_angle ? 0 : 1; });
        }
    }

    glm::vec3 GetUnitNormal(const glm::vec2 &p1, const glm::vec2 &p2, const float s)
    {
        const glm::vec2 p3 = p1 + ((p2 - p1) * s);
        const float m = (p3.y - p1.y) / (p3.x - p1.x);
        const float c = p1.y - m * p1.x;
        const float y = (m * p1.x) + c;

        const glm::vec2 tangent = glm::normalize(glm::vec2(p1.x, y));
        glm::vec3 normal = glm::vec3(-tangent.y, 0, tangent.x);

        return glm::normalize(normal);
    }

    void Brush::generatePolygons()
    {
        this->polygons.resize(faces.size());
        for (auto &p : this->polygons)
        {
            p = std::make_shared<Polygon>();
        }
        float phongAngle = 89.0;

        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0; j < faces.size(); j++)
                for (int k = 0; k < faces.size(); k++)
                {
                    if (i == j && i == k && j == k)
                        continue;

                    auto res = intersectPlanes(faces[i], faces[j], faces[k]);
                    if (!res.first || !res.second.checkLegalVertext(faces))
                        continue;

                    res.second = mergeDuplicate(i, res.second);

                    auto v = res.second;
                    v.normal = faces[i].planeNormal;
                    v.normal = glm::normalize(v.normal);

                    polygons[k]->faceRef = faces[k];
                    v.tangent = QMath::CalcTangent(hasValveUV, polygons[k]->faceRef);

                    if (v.inList(polygons[k]->vertices))
                        continue;
                    polygons[k]->vertices.push_back(v);
                }
        }
    }

    const bool Brush::DoesIntersect(Brush &other)
    {
        if ((min.x > other.max.x) || (other.min.x > max.x))
            return false;

        if ((min.y > other.max.y) || (other.min.y > max.y))
            return false;

        if ((min.z > other.max.z) || (other.min.z > max.z))
            return false;

        return true;
    }

    void Brush::calculateAABB()
    {
        min = polygons[0]->vertices[0].point;
        max = polygons[0]->vertices[0].point;

        for (const auto &face : polygons)
            for (const auto &vert : face->vertices)
            {
                if (vert.point.x < min.x)
                    min.x = vert.point.x;

                if (vert.point.y < min.y)
                    min.y = vert.point.y;

                if (vert.point.z < min.z)
                    min.z = vert.point.z;

                if (vert.point.x > max.x)
                    max.x = vert.point.x;

                if (vert.point.y > max.y)
                    max.y = vert.point.y;

                if (vert.point.z > max.z)
                    max.z = vert.point.z;
            }
    }
}