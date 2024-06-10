#pragma once

#include <cmath>
#include "types.h"

namespace qformats::map
{

	static std::ostream& operator<<(std::ostream& stream, const fvec3& v)
	{
		stream << "(" << v[0] << " " << v[1] << " " << v[2] << ")";
		return stream;
	}

	const float CMP_EPSILON = 0.008;
#ifndef M_PI
	const float M_PI = 3.14159265358979323846;
#endif
	const auto UP_VEC = fvec3{ 0, 0, 1 };
	const auto RIGHT_VEC = fvec3{ 0, 1, 0 };
	const auto FORWARD_VEC = fvec3{ 1, 0, 0 };
}