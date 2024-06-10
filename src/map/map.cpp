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

	void QMap::GenerateGeometry(bool clipBrushes)
	{
		for (const auto &se : map_file->solidEntities)
		{
			se->generateMesh(excludedTextureIDs);
			if (clipBrushes)
			{
				se->csgUnion();
			}
		}
	}

	void QMap::LoadTextures(textures::textureRequestCb cb)
	{
		texMan.OnTextureRequested(cb);

		for (auto t : map_file->textures)
		{
			texMan.GetOrAddTexture(t);
		}

		for (auto &se : map_file->solidEntities)
		{
			auto &brushes = se->clippedBrushes.empty() ? se->brushes : se->clippedBrushes;
			for (auto &b : brushes)
			{
				for (auto p : b.GetFaces())
				{
					if (p->vertices.size() == 0)
					{
						continue;
					}
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
	}

	void QMap::ExcludeTextureSurface(const std::string &texName)
	{
		if (map_file == nullptr)
			return;
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
		{
			return false;
		}

		for (auto &b : map_file->solidEntities[entityID].get()->brushes)
		{
			for (auto &p : b.GetFaces())
			{
				if (p->textureID == texID)
				{
					list.push_back(p);
				}
			}
		}
		return !list.empty();
	}

	std::vector<FacePtr> QMap::GetPolygonsByTexture(int entityID, const std::string &texName)
	{
		int id = texMan.GetTextureID(texName);
		std::vector<FacePtr> polyList;
		if (id == -1)
		{
			return polyList;
		}

		getPolygonsByTextureID(entityID, id, polyList);
		return polyList;
	}

	void QMap::GatherPolygons(int entityID, const polygonGatherCb &cb)
	{
		if (map_file->solidEntities.size() >= entityID || entityID < 0)
		{
			return;
		}

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
