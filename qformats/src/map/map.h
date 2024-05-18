//
// Created by tgoeh on 10.11.2023.
//
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "types.h"
#include "map_file.h"
#include "brush.h"
#include "../common/textureman.h"

namespace qformats::map
{
    class SolidEntity
    {
    public:
        QBrushEntity *entityRef;
        std::vector<Brush> geoBrushes;
        std::vector<int> textureIDs;
    };

    class QMap
    {
    public:
        QMap() = default;
        ~QMap() = default;

        void LoadTextures(textures::textureRequestCb cb);
        void LoadFile(const std::string &filename);
        void GenerateGeometry();
        void MergeBrushes();
        textures::ITexture *GetTextureByID(int id) { return texMan.GetTexture(id); };
        const std::vector<textures::ITexture *> GetTextures() { return texMan.GetTextures(); };

        QMapFile *MapData() { return map_file; };
        std::vector<QBrushEntity *> &BrushEntities() { return map_file->brushEntities; };
        std::vector<SolidEntity> solidEntities;

    private:
        textures::TextureMan texMan;
        QMapFile *map_file;
    };
}
