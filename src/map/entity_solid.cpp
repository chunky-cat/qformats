#include "qformats/map/entities.h"

namespace qformats::map
{

	void SolidEntity::generateMesh(const std::map<int, Face::eFaceType>& faceTypes)
	{
		if (!brushes.empty())
		{
			min = brushes[0].min;
			max = brushes[0].max;
		}
		for (auto& b : brushes)
		{
			b.buildGeometry(faceTypes);
			b.GetBiggerBBox(min,max);
		}
		center = CalculateCenterFromBBox(min,max);
	}

	void SolidEntity::csgUnion()
	{
		if (!brushes.empty())
		{
			min = brushes[0].min;
			max = brushes[0].max;
		}
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
				cpBrush.GetBiggerBBox(min,max);
				stats_clippedFaces += b1.faces.size() - cpBrush.faces.size();
			}
		}
		center = CalculateCenterFromBBox(min,max);
	}
}