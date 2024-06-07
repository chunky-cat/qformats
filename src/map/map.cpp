#include "qformats/map/map.h"
#include <qformats/map/qmath.h>

#include <iostream>

namespace qformats::map
{
    void QMap::LoadFile(const std::string &filename)
    {
        auto qfile = new QMapFile();
        qfile->Parse(filename);
        map_file = qfile;
    }

    void QMap::GenerateGeometry()
    {
        for (const auto &se : map_file->solidEntities)
        {
            se->generateMesh(excludedTextureIDs);
            se->csgUnion();
        }
        //ConstructiveSolidGeometryUnion();
    }

    /*
    void QMap::ConstructiveSolidGeometryUnion()
    {
        for (auto& se : map_file->solidEntities)
        {
            for (auto& b : se.brushes)
            {
                std::cout << "-- BRUSH --" << std::endl;
                for (auto &p : b.polygons)
                {
                    for (auto& v : p->vertices)
                    {
                        std::cout << v << std::endl;
                    }
                    std::cout << "--------" << std::endl;
                }
            }

            auto clippedBrushes = se.brushes;

            for (size_t i = 0; i < se.brushes.size(); i++)
            {
                for (size_t j = 0; j < se.brushes.size(); j++)
                {
                    if (i == j)
                    {
                        std::cout << "SKIP Brush" << std::endl;
                        continue;
                    }

                    if (clippedBrushes[i].DoesIntersect(se.brushes[j]))
                    {
                        std::cout << "intersects" << std::endl;
                        clippedBrushes[i].clipToBrush(se.brushes[j]);
                    }
                }
            }
        }
    }
     */

    void QMap::LoadTextures(textures::textureRequestCb cb)
    {
        texMan.OnTextureRequested(cb);

        for (auto t : map_file->textures)
        {
            texMan.GetOrAddTexture(t);
        }

        for (auto &se : map_file->solidEntities)
            for (auto &b : se->brushes)
            {
                for (auto p : b.faces)
                {
                    if (p->vertices.size() == 0) continue;
                    auto tex = texMan.GetTexture(p->textureID);
                    if (tex == nullptr)
                    {
                        continue;
                    }

                    for (auto &v : p->vertices)
                    {
                        v.uv = p->CalcUV(v.point, tex->Width(), tex->Height());
                    }
                }
            }
    }

    void QMap::ExcludeTextureSurface(const std::string& texName)
    {
        for (int i = 0; i < map_file->textures.size(); i++)
        {
            if (map_file->textures[i].find(texName) != std::string::npos)
            {
                excludedTextureIDs[i] = true;
                return;
            }
        }
    }

    std::vector<PointEntityPtr> QMap::GetPointEntitiesByClass(const std::string &className)
    {
        std::vector<PointEntityPtr> ents;
        for (auto pe : map_file->pointEntities)
        {
            if (pe->classname.find(className) != std::string::npos)
            {
                ents.push_back(pe);
            }
        }
        return ents;
    }

    bool QMap::getPolygonsByTextureID(int entityID, int texID, std::vector<FacePtr> &list)
    {
        if (map_file->solidEntities.size() >= entityID || entityID < 0)
            return false;

        for (auto &b : map_file->solidEntities[entityID].get()->brushes)
            for (auto &p : b.faces)
            {
                if (p->textureID == texID)
                    list.push_back(p);
            }
        return list.size() > 0;
    }

    std::vector<FacePtr> QMap::GetPolygonsByTexture(int entityID, std::string texName)
    {
        int id = texMan.GetTextureID(std::move(texName));
        std::vector<FacePtr> polyList;
        if (id == -1)
            return polyList;

        getPolygonsByTextureID(entityID, id, polyList);
        return polyList;
    }

    void QMap::GatherPolygons(int entityID, polygonGatherCb cb)
    {
        if (map_file->solidEntities.size() >= entityID || entityID < 0)
            return;

        for (int i = 0; i < map_file->textures.size(); i++)
        {
            std::vector<FacePtr> polyList;
            if (getPolygonsByTextureID(entityID, i, polyList))
            {
                cb(polyList, i);
            }
        }
    }
}
