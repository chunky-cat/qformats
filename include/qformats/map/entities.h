#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "types.h"
#include "brush.h"

namespace qformats::map {

    class BaseEntity
    {
    public:
        std::string classname;
        float angle;
        std::map<std::string, std::string> attributes;

        friend class QMapFile;
    };

    class PointEntity : public BaseEntity
    {
    public:
        fvec3 origin{};

        friend class QMapFile;
    };

    class SolidEntity: public BaseEntity {
    public:
        [[nodiscard]] std::string ClassName() const { return classname; };
        [[nodiscard]] bool ClassContains(const std::string &substr) const
        {
            return classname.find(substr) != std::string::npos;
        };
        const std::vector<Brush> &GetBrushes() { return brushes; }
        const std::vector<Brush> &GetClippedBrushes() { return clippedBrushes; }
        // stats getter
        size_t  StatsClippedFaces() { return stats_clippedFaces; }
    private:
        void generateMesh(const std::map<int, bool> &excludedTextureIDs);
        void csgUnion();

        std::vector<Brush> brushes;
        std::vector<Brush> clippedBrushes;
        std::string tbType;
        std::string tbName;
        bool hasPhongShading;
        std::vector<int> textureIDs;
        size_t stats_clippedFaces;

        friend class QMapFile;
        friend class QMap;
    };

    using BaseEntityPtr = std::shared_ptr<BaseEntity>;
    using SolidEntityPtr = std::shared_ptr<SolidEntity>;
    using PointEntityPtr = std::shared_ptr<PointEntity>;
}

