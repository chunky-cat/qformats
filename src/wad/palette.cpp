#include "qformats/wad/palette.h"

#include <fstream>
#include <iterator>
#include <algorithm>

namespace qformats::wad
{
	color Palette::GetColor(int index)
	{
		if (index < 0 || index >= colors.size())
		{
			throw std::runtime_error("color index out of range");
		}

		return colors[index];
	}

	cvec Palette::GetBrightColors()
	{
		auto start = colors.begin() + 240;
		auto end = colors.begin() + 254;
		cvec result(254 - 240 + 1);
		std::copy(start, end, result.begin());
		return result;
	}
}