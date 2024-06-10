#include "qformats/map/entities.h"

namespace qformats::map
{

	void SolidEntity::generateMesh(const std::map<int, bool>& excludedTextureIDs)
	{
		for (auto& b : brushes)
		{
			b.buildGeometry(excludedTextureIDs);
		}
	}

	void SolidEntity::csgUnion()
	{
		for (auto& b1 : brushes)
		{
			auto cpBrush = b1;
			for (auto& b2 : brushes)
			{
				if (&b1 == &b2 || b2.faces.empty())
				{
					continue;
				}

				if (!b1.DoesIntersect(b2))
				{
					continue;
				}

				auto clippedFaces = cpBrush.clipToBrush(b2);
				cpBrush.faces = clippedFaces;

			}
			if (!cpBrush.faces.empty())
			{
				clippedBrushes.push_back(cpBrush);
				stats_clippedFaces += b1.faces.size() - cpBrush.faces.size();
			}
		}
	}
}