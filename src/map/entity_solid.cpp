#include "qformats/map/entities.h"


namespace qformats::map {

    void SolidEntity::generateMesh(const std::map<int, bool> &excludedTextureIDs) {

        for (auto &b: brushes) {
            b.buildGeometry(excludedTextureIDs);
        }
    }

    void SolidEntity::csgUnion() {
        for (auto &b1: brushes) {
            for (auto &b2: brushes) {
                if (&b1 == &b2) continue;
                if (!b1.DoesIntersect(b2)) continue;
                b1.clipToBrush(b2);
                std::cout << "next brush" << std::endl;

            }
        }
    }
}