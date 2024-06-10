#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>

#include "default_pal.h"

namespace qformats::wad
{
	struct color
	{
		unsigned char rgba[4] = { 0, 0, 0, 1 };
	};

	using cvec = std::vector<color>;
// 240
	class Palette
	{
	public:
		static Palette FromFile(const std::string& fileName)
		{
			std::ifstream input(fileName, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return FromBuffer(buffer.data(), buffer.size());
		}

		static Palette FromBuffer(const unsigned char* buff, int size)
		{
			auto p = Palette();
			if (size % 3 != 0)
			{
				throw std::runtime_error("color buffer malformed");
			}
			p.colors.resize(size / 3);
			int col_i = 0;
			int i = 0;
			for (i = 0; i < size; i += 3)
			{
				p.colors[col_i].rgba[0] = buff[i];
				p.colors[col_i].rgba[1] = buff[i + 1];
				p.colors[col_i].rgba[2] = buff[i + 2];
				p.colors[col_i].rgba[3] = 255;
				if (col_i == 255)
					p.colors[col_i].rgba[3] = 0;
				col_i++;
			}

			return p;
		}
		color GetColor(int index);
		cvec GetBrightColors();

	private:
		cvec colors;
	};
	static Palette default_palette = Palette::FromBuffer(default_palette_lmp, default_palette_size);
}