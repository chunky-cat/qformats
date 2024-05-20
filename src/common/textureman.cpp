#include "qformats/textureman.h"
#include <iostream>

namespace qformats::textures
{
    int TextureMan::GetOrAddTexture(string name)
    {
        for (int i = 0; i < textures.size(); i++)
        {
            if (name == textures[i]->name)
            {
                return i;
            }
        }
        auto newTex = invokeTextureRequest(name);
        if (newTex != nullptr)
        {
            textures.push_back(newTex);
            return textures.size() - 1;
        }

        return -1;
    }

    ITexture *TextureMan::GetTexture(int texID)
    {
        if (textures.size() <= texID)
            return nullptr;

        return textures[texID];
    }

    int TextureMan::GetTextureID(std::string name)
    {
        for (int i = 0; i < textures.size(); i++)
        {
            if (name == textures[i]->name)
            {
                return i;
            }
        }
        return -1;
    }

    void TextureMan::OnTextureRequested(textureRequestCb cb)
    {
        reqCb = cb;
    }

    ITexture *TextureMan::invokeTextureRequest(string name)
    {
        auto newTex = reqCb(name);
        if (newTex != nullptr)
            newTex->name = name;

        return newTex;
    }
}