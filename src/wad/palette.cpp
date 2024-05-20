#include "qformats/wad/palette.h"

#include <fstream>
#include <iterator>
#include <algorithm>

namespace qformats::wad
{
    Palette Palette::FromFile(const std::string &fileName)
    {
        std::ifstream input(fileName, std::ios::binary);
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
        input.close();
        return FromBuffer(buffer.data(), buffer.size());
    }

    Palette Palette::FromBuffer(const unsigned char *buff, int size)
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