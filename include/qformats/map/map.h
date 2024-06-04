//
// Created by tgoeh on 10.11.2023.
//
#pragma once

#include <string>
#include <vector>
#include <functional>

#include "types.h"
#include "map_file.h"
#include "brush.h"
#include "../textureman.h"

namespace qformats::map
{
    class SolidEntity
    {
    public:
        std::string ClassName() const { return entityRef->classname; };
        bool ClassContains(const std::string &substr) const
        {
            return entityRef->classname.find(substr) != std::string::npos;
        };

        QBrushEntity *entityRef;
        std::vector<Brush> geoBrushes;
        std::vector<int> textureIDs;
    };

    using polygonGatherCb = std::function<void(std::vector<PolygonPtr>, int)>;

    class QMap
    {
    public:
        QMap() = default;
        ~QMap() = default;

        void LoadTextures(textures::textureRequestCb cb);
        void LoadFile(const std::string &filename);
        void GenerateGeometry();
        void GatherPolygons(int entityID, polygonGatherCb);
        std::vector<PolygonPtr> GetPolygonsByTexture(int entityID, std::string texName);
        const std::vector<string> &Wads() { return map_file->wads; };
        bool HasWads() { return map_file->wads.size() > 0; };
        textures::ITexture *GetTextureByID(int id) { return texMan.GetTexture(id); };
        QMapFile *MapData() { return map_file; };
        std::vector<QPointEntity *> GetPointEntitiesByClass(const std::string &className);
        const std::vector<QPointEntity *> &GetPointEntities() { return map_file->pointEntities; };

        void ExcludeTextureSurface(std::string texture);
        qformats::map::QPointEntity *GetPlayerStart();
        const std::vector<SolidEntity> &GetSolidEntities() { return solidEntities; };
        const std::vector<textures::ITexture *> GetTextures() { return texMan.GetTextures(); };

    private:
        bool getPolygonsByTextureID(int entityID, int texID, std::vector<PolygonPtr> &list);

        std::map<int, bool> excludedTextureIDs;
        std::vector<SolidEntity> solidEntities;
        textures::TextureMan texMan;
        QMapFile *map_file;
    };
}
