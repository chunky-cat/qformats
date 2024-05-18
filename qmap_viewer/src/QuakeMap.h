#pragma once

#include <raylib.h>
#include <map/map.h>
#include <string>
#include <vector>
#include <map>

struct QuakeModel
{
    Model model;
    std::vector<Mesh> meshes;
    std::vector<int> materialIDs;
};

struct QuakeMapOptions
{
    std::string texturePath = "testdata/textures/";
    int inverseScale = 24;
    Color defaultColor = WHITE;
    Color WireTint = RED;
    Color WireModelTint = ColorAlpha(Color{100, 100, 255}, 0.4);
};

enum QM_RENDER_MODE
{
    QM_TEXTURED = 0,
    QM_WIRERFAME = 1,
};

class QuakeMap
{
public:
    QuakeMap(QuakeMapOptions opts) : opts(opts){};
    void DrawQuakeSolids();
    void LoadMapFromFile(std::string fileName);
    void ChangeRenderMode(QM_RENDER_MODE mode);
    QM_RENDER_MODE renderMode;

private:
    QuakeModel readModelFromBrushes(const std::vector<qformats::map::Brush> &brushes);
    qformats::textures::ITexture *onTextureRequest(std::string name);
    QuakeMapOptions opts;

    qformats::map::QMap mapInstance;
    Material *materialPool;
    std::vector<QuakeModel> models;
    Material defaultMaterial;
};