#include "qformats/map/map.h"
#include "qmath.h"
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
        for (const auto &e : map_file->brushEntities)
        {
            SolidEntity se;
            se.entityRef = e;
            for (auto &b : e->brushes)
            {
                b.buildGeometry(excludedTextureIDs);
                se.geoBrushes.push_back(b);
            }
            solidEntities.push_back(se);
        }
    }

    void QMap::LoadTextures(textures::textureRequestCb cb)
    {
        texMan.OnTextureRequested(cb);

        for (auto t : map_file->textures)
        {
            texMan.GetOrAddTexture(t);
        }

        for (auto &se : solidEntities)
            for (auto &b : se.geoBrushes)
            {
                for (auto p : b.polygons)
                {
                    if (p->vertices.size() == 0) continue;
                    auto tex = texMan.GetTexture(p->faceRef.textureID);
                    if (tex == nullptr)
                    {
                        continue;
                    }

                    for (auto &v : p->vertices)
                    {
                        v.uv = QMath::CalcUV(b.hasValveUV, v.point, p->faceRef, tex->Width(), tex->Height());
                    }
                }
            }
    }

    void QMap::ExcludeTextureSurface(std::string texName)
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

    std::vector<QPointEntity *> QMap::GetPointEntitiesByClass(const std::string &className)
    {
        std::vector<QPointEntity *> ents;
        for (auto pe : map_file->pointEntities)
        {
            if (pe->classname.find(className) != std::string::npos)
            {
                ents.push_back(pe);
            }
        }
        return ents;
    }

    bool QMap::getPolygonsByTextureID(int entityID, int texID, std::vector<PolygonPtr> &list)
    {
        if (solidEntities.size() >= entityID || entityID < 0)
            return false;

        for (auto &b : solidEntities[entityID].geoBrushes)
            for (auto &p : b.polygons)
            {
                if (p->faceRef.textureID == texID)
                    list.push_back(p);
            }

        return list.size() > 0;
    }

    std::vector<PolygonPtr> QMap::GetPolygonsByTexture(int entityID, std::string texName)
    {
        int id = texMan.GetTextureID(texName);
        std::vector<PolygonPtr> polyList;
        if (id == -1)
            return polyList;

        getPolygonsByTextureID(entityID, id, polyList);
        return polyList;
    }

    void QMap::GatherPolygons(int entityID, polygonGatherCb cb)
    {
        if (solidEntities.size() >= entityID || entityID < 0)
            return;

        for (int i = 0; i < map_file->textures.size(); i++)
        {
            std::vector<PolygonPtr> polyList;
            if (getPolygonsByTextureID(entityID, i, polyList))
            {
                cb(polyList, i);
            }
        }
    }
}
