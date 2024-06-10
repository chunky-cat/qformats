#pragma once

#include "types.h"
#include "qmath.h"
#include <memory>
#include <tuple>

namespace qformats::map
{
	class Face;
	using FacePtr = std::shared_ptr<Face>;
	using FaceIter = std::vector<FacePtr>::const_iterator;

	class Face
	{
	public:
		enum eCF
		{
			FRONT = 0,
			BACK,
			ON_PLANE,
			SPANNING,
		};

	public:
		Face() = default;
		Face(const std::array<fvec3, 3>& points, int textureID, StandardUV uv, float rotation, float scaleX,
				float scaleY)
				:planePoints(points), textureID(textureID), standardUV(uv), rotation(rotation), scaleX(scaleX),
				 scaleY(scaleY), hasValveUV(false)
		{
			initPlane();
		};
		Face(const std::array<fvec3, 3>& points, int textureID, ValveUV uv, float rotation, float scaleX,
				float scaleY)
				:planePoints(points), textureID(textureID), valveUV(uv), rotation(rotation), scaleX(scaleX),
				 scaleY(scaleY), hasValveUV(true)
		{
			initPlane();
		};
		Face::eCF Classify(const Face* other);
		Face::eCF ClassifyPoint(const fvec3& v);
		void UpdateAB();
		void UpdateNormals();
		[[nodiscard]] FacePtr Copy() const;

		[[nodiscard]] int TextureID() const
		{
			return textureID;
		};

		[[nodiscard]] inline const fvec3 &GetPlaneNormal() const { return planeNormal; }
		[[nodiscard]] inline const float &GetPlaneDist() const { return planeDist; }
		[[nodiscard]] inline const std::vector<Vertex> &GetVertices() const { return vertices; }
		[[nodiscard]] inline const std::vector<unsigned short> &GetIndices() const { return indices; }

		fvec3 center{}, min{}, max{};
		bool noDraw = false;
		bool operator==(const Face& arg_) const;

		friend std::ostream&
		operator<<(std::ostream& stream, const Face& p)
		{
			stream << "polygon: ";
			for (auto& v : p.vertices)
			{
				stream << v << "  ";
			}

			stream << p.planeNormal;

			return stream;
		}

	private:
		fvec4 CalcTangent()
		{
			return hasValveUV ? calcValveTangent() : calcStandardTangent();
		};
		fvec2 CalcUV(fvec3 vertex, int texW, int texH)
		{
			return hasValveUV ? calcValveUV(vertex, texW, texH) : calcStandardUV(vertex, texW, texH);
		};

		void initPlane();
		fvec4 calcStandardTangent();
		fvec4 calcValveTangent();
		fvec2 calcStandardUV(fvec3 vertex, int texW, int texH);
		fvec2 calcValveUV(fvec3 vertex, int texW, int texH);
		bool getIntersection(const fvec3& start, const fvec3& end, fvec3& out_intersectionPt, float& out_percentage);
		std::pair<FacePtr, FacePtr> splitFace(const Face* other);

		std::array<fvec3, 3> planePoints{};
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
		fvec3 planeNormal{};
		float planeDist{};
		StandardUV standardUV{};
		ValveUV valveUV{};
		float rotation{};
		float scaleX{};
		float scaleY{};
		bool hasValveUV{};
		int textureID{};

		friend class Brush;
		friend class QMap;
	};
}