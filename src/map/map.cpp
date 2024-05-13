#include "map.h"
#include <iostream>
#include <algorithm> // std::sort

namespace qformats::map
{
    bool inList(std::vector<glm::vec3> &list, glm::vec3 &vert);

    QMap *QMap::FromFIle(const std::string &filename)
    {
        auto qfile = new QMapFile();
        qfile->Parse(filename);
        auto res = new QMap();
        res->map_file = qfile;
        res->generatePolygons();
        return res;
    }

    void QMap::generatePolygons()
    {
        if (map_file == nullptr)
            return;

        for (auto &be : map_file->brushEntities)
        {
            for (auto &br : be->brushes)
            {
                generatePolygonsForBrush(br);
            }
        }
    }

    void QMap::generatePolygonsForBrush(QBrush *brush)
    {
        brush->polys.resize(brush->faces.size());
        for (int i = 0; i < brush->faces.size(); i++)
        {
            for (int j = 0; j < brush->faces.size(); j++)
                for (int k = 0; k < brush->faces.size(); k++)
                {
                    if (i == j && i == k && j == k)
                        continue;

                    auto res = intersectPlanes(brush->faces[i], brush->faces[j], brush->faces[k]);

                    if (!res.first || !checkLegalVertext(res.second, brush->faces))
                        continue;

                    // If we already generated a vertex close enough to this one, then merge them.
                    bool merged = false;
                    auto vertex = res.second;
                    for (int n = 0; n <= i; n++)
                    {
                        auto otherPoly = &brush->polys[k];
                        for (int m = 0; m < otherPoly->vertices.size(); m++)
                        {
                            if (glm::distance(otherPoly->vertices[m], res.second) < 0.0008)
                            {
                                vertex = otherPoly->vertices[m];
                                merged = true;
                                break;
                            }
                        }
                        if (merged)
                        {
                            break;
                        }
                    }

                    if (!inList(brush->polys[k].vertices, vertex))
                    {
                        brush->polys[k].face = brush->faces[k];
                        brush->polys[k].vertices.push_back(vertex);
                        brush->vertCount++;
                    }
                }
        }

        for (auto &poly : brush->polys)
        {
            for (auto vert : poly.vertices)
            {
                poly.center += vert;
                std::cout << std::printf("vert (%f %f %f)", vert.x, vert.y, vert.z) << std::endl;
            }
            std::cout << poly.vertices.size() << std::endl;
            poly.center /= poly.vertices.size();
        }

        windFaceVertices2();

        for (int p = 0; p < brush->polys.size(); p++)
        {
            auto poly = &brush->polys[p];
            if (poly->vertices.size() < 3)
                continue;

            poly->indices.resize((poly->vertices.size() - 2) * 3);
            for (int i = 0; i < poly->vertices.size() - 2; i++)
            {
                poly->indices.push_back(0);
                poly->indices.push_back(i + 1);
                poly->indices.push_back(i + 2);
            }
            brush->indiceCount += poly->indices.size();
        }
    }

    bool inList(std::vector<glm::vec3> &list, glm::vec3 &vert)
    {
        for (auto const &v : list)
        {
            if (v == vert)
            {
                return true;
            }
        }
        return false;
    }

    void QMap::windFaceVertices()
    {
        for (auto &be : this->map_file->brushEntities)
        {
            for (int b = 0; b < be->brushes.size(); b++)
            {
                for (auto &p : be->brushes[b]->polys)
                {
                    glm::vec3 c = p.center + (float)1e-5; // in case the angle of every vertex is the same, the center  should be slightly off
                    glm::vec3 n = p.face.planeNormal;
                    std::stable_sort(p.vertices.begin(), p.vertices.end(), [&](glm::vec3 lhs, glm::vec3 rhs)
                                     {
                        if (lhs == rhs)
                            return false; // this line makes this an unstable sort
                        
                        glm::vec3 ca = c - lhs;
                        glm::vec3 cb = c - rhs;
                        glm::vec3 caXcb = glm::cross(ca,cb);
                        
                        return glm::dot(n,caXcb) >= 0; });
                }
            }
        }
    }

    void QMap::windFaceVertices2()
    {
        std::cout << "after winding" << std::endl;

        for (int e = 0; e < map_file->brushEntities.size(); e++)
        {
            for (int b = 0; b < map_file->brushEntities[e]->brushes.size(); b++)
            {
                auto faceSpan = map_file->brushEntities[e]->brushes[b]->faces;
                auto faceGeoSpan = map_file->brushEntities[e]->brushes[b]->polys;
                for (int f = 0; f < faceSpan.size(); f++)
                {
                    auto face = faceSpan[f];
                    auto vertexSpan = faceGeoSpan[f].vertices;

                    if (vertexSpan.size() < 3)
                        continue;

                    auto windFaceBasis = glm::normalize(vertexSpan[1] - vertexSpan[0]);
                    auto windFaceCenter = glm::vec3();
                    auto windFaceNormal = glm::normalize(face.planeNormal);

                    for (int v = 0; v < vertexSpan.size(); v++)
                    {
                        windFaceCenter += vertexSpan[v];
                    }
                    windFaceCenter /= (float)vertexSpan.size();

                    std::stable_sort(vertexSpan.begin(), vertexSpan.end(), [&](glm::vec3 l, glm::vec3 r)
                                     {

                            glm::vec3 u = glm::normalize(windFaceBasis);
                            glm::vec3 v = glm::normalize(glm::cross(u, windFaceNormal));

                            glm::vec3 loc_a = l - windFaceCenter;
                            float a_pu = glm::dot(loc_a, u);
                            float a_pv = glm::dot(loc_a, v);

                            glm::vec3 loc_b = r - windFaceCenter;
                            float b_pu = glm::dot(loc_b, u);
                            float b_pv = glm::dot(loc_b, v);

                            float a_angle = atan2(a_pv, a_pu);
                            float b_angle = atan2(b_pv, b_pu);

                            if (a_angle == b_angle) {
                                return 0;
                            }
                            std::cout << (a_angle > b_angle) << std::endl; 
                            return a_angle > b_angle ? 0 : 1; });

                    map_file->brushEntities[e]->brushes[b]->polys[f].vertices = vertexSpan;

                    for (auto vert : vertexSpan)
                        std::cout << std::printf("vert (%f %f %f)", vert.x, vert.y, vert.z) << std::endl;
                    std::cout << vertexSpan.size() << std::endl;
                }
            }
        }
    }

    bool QMap::checkLegalVertext(glm::vec3 vert, std::vector<QFace> faces)
    {
        for (auto f : faces)
        {
            auto proj = glm::dot(f.planeNormal, vert);
            if (proj > f.planeDist && abs(f.planeDist - proj) > 0.0008)
            {
                return false;
            }
        }
        return true;
    }

    std::pair<bool, glm::vec3> QMap::intersectPlanes(QFace &a, QFace &b, QFace &c)
    {
        glm::vec3 n0 = a.planeNormal;
        glm::vec3 n1 = b.planeNormal;
        glm::vec3 n2 = c.planeNormal;

        /*

                    float denom = Vector3.Dot(Vector3.Cross(n0, n1), n2);
                    if (denom < CMP_EPSILON) return null;

                    var v = (Vector3.Cross(n1, n2) * f0.planeDist + Vector3.Cross(n2, n0) * f1.planeDist + Vector3.Cross(n0, n1) * f2.planeDist) / denom;
                    return v;
        */

        float denom = glm::dot(glm::cross(n0, n1), n2);
        if (denom < 0.008)
            return std::pair<bool, glm::vec3>(false, glm::vec3());

        auto v = (glm::cross(n1, n2) * a.planeDist + glm::cross(n2, n0) * b.planeDist + glm::cross(n0, n1) * c.planeDist) / denom;
        return std::pair<bool, glm::vec3>(true, v);
    }
}
