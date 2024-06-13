#pragma once

#include "types.h"
#include "face.h"
#include <vector>
#include <memory>

namespace qformats::map
{
	template<class T>
	using boolRet = std::pair<bool, T>;
	class Brush
	{
	public:
		Brush() = default;
		bool DoesIntersect(const Brush& other);
		void buildGeometry(const std::map<int, bool>& excludedTextureIDs);
		[[nodiscard]] inline const std::vector<FacePtr> &GetFaces() const { return faces; }
		fvec3 GetMin() const { return min; };
		fvec3 GetMax() const { return max; };
		
		fvec3 min{};
		fvec3 max{};
	private:
		std::vector<FacePtr> faces;

		void generatePolygons(const std::map<int, bool>& excludedTextureIDs);
		void windFaceVertices();
		std::vector<FacePtr> clipToBrush(const Brush& other);
		void indexFaceVertices();
		void calculateAABB();
		Vertex mergeDuplicate(int from, Vertex& v);
		boolRet<Vertex> intersectPlanes(const FacePtr& a, const FacePtr& b, const FacePtr& c);
		static bool isLegalVertex(const Vertex& v, const std::vector<FacePtr>& faces);

		FacePtr clipToList(FaceIter first, const FaceIter& firstEnd, FaceIter second, const FaceIter& secondEnd);

		friend class QMapFile;
		friend class SolidEntity;
	};
}
