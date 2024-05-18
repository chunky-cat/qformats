#include "QuakeMap.h"
#include <raymath.h>

qformats::textures::ITexture *QuakeMap::onTextureRequest(std::string name)
{
    auto rayTex = LoadTexture((opts.texturePath + name + ".png").c_str());
    if (rayTex.id == -1)
        return nullptr;
    auto tex = new qformats::textures::Texture<Material>();

    Material mat = LoadMaterialDefault();

    mat.maps->texture = rayTex;
    mat.maps->color = WHITE;
    tex->SetData(mat);
    tex->SetWidth(rayTex.width);
    tex->SetHeight(rayTex.height);
    return tex;
}

void QuakeMap::DrawQuakeSolids()
{
    Vector3 position = {0.0f, 0.0f, 0.0f};
    for (const auto &m : models)
    {
        switch (renderMode)
        {
        case QM_WIRERFAME:
            DrawModel(m.model, position, 1, opts.WireModelTint);
            DrawModelWires(m.model, position, 1, opts.WireTint);
            break;

        default:
            DrawModel(m.model, position, 1, WHITE);
            break;
        }
    }
}

void QuakeMap::ChangeRenderMode(QM_RENDER_MODE mode)
{
    switch (renderMode)
    {
    case QM_WIRERFAME:
    {
        for (auto &qm : models)
        {
            qm.model.materials = &defaultMaterial;
            for (int m = 0; m < qm.meshes.size(); m++)
                qm.model.meshMaterial[m] = 0;
        }
        break;
    }

    default:
    {
        for (auto &qm : models)
        {
            qm.model.materials = materialPool;
            for (int m = 0; m < qm.meshes.size(); m++)
                qm.model.meshMaterial[m] = qm.materialIDs[m];
        }
        break;
    }
    }
}

void QuakeMap::LoadMapFromFile(std::string fileName)
{
    mapInstance = qformats::map::QMap();
    mapInstance.LoadFile(fileName);
    mapInstance.GenerateGeometry();
    mapInstance.LoadTextures([this](std::string name) -> qformats::textures::ITexture *
                             { return this->onTextureRequest(name); });

    auto textures = mapInstance.GetTextures();

    defaultMaterial = LoadMaterialDefault();
    defaultMaterial.maps[MATERIAL_MAP_DIFFUSE].color = opts.defaultColor;

    materialPool = (Material *)MemAlloc(mapInstance.GetTextures().size() * sizeof(Material));
    for (int i = 0; i < textures.size(); i++)
    {
        materialPool[i] = ((qformats::textures::Texture<Material> *)(textures[i]))->Data();
    }

    for (const auto &se : mapInstance.solidEntities)
    {
        auto m = readModelFromBrushes(se.geoBrushes);
        models.push_back(m);
    }
}

QuakeModel QuakeMap::readModelFromBrushes(const std::vector<qformats::map::Brush> &brushes)
{
    QuakeModel qm;
    for (const auto &b : brushes)
    {
        for (const auto &p : b.polygons)
        {
            int i = 0;
            int i_indices = 0;
            int i_uv = 0;

            auto mesh = Mesh{0};
            mesh.triangleCount = p->indices.size() / 3;
            mesh.vertexCount = p->vertices.size();
            mesh.indices = (ushort *)MemAlloc(p->indices.size() * sizeof(ushort));
            mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float)); // 3 vertices, 3 coordinates each (x, y, z)
            mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
            mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float)); // 3 vertices, 3 coordinates each (x, y, z)

            for (const auto &v : p->vertices)
            {
                mesh.vertices[i] = v.point.x / opts.inverseScale;
                mesh.normals[i] = v.normal.x;
                i++;
                mesh.vertices[i] = v.point.z / opts.inverseScale;
                mesh.normals[i] = v.normal.z;
                i++;
                mesh.vertices[i] = v.point.y / opts.inverseScale;
                mesh.normals[i] = v.normal.y;
                i++;
                mesh.texcoords[i_uv++] = v.uv.x;
                mesh.texcoords[i_uv++] = v.uv.y;
            }
            for (auto idx : p->indices)
            {
                mesh.indices[i_indices] = idx;
                i_indices++;
            }

            UploadMesh(&mesh, false);
            qm.meshes.push_back(mesh);
            qm.materialIDs.push_back(p->faceRef.textureID);
        }
    }

    qm.model.transform = MatrixIdentity();
    qm.model.meshCount = qm.meshes.size();
    qm.model.materialCount = mapInstance.GetTextures().size();
    qm.model.meshes = (Mesh *)MemAlloc(qm.model.meshCount * sizeof(Mesh));
    qm.model.materials = materialPool;
    qm.model.meshMaterial = (int *)MemAlloc(qm.model.meshCount * sizeof(int));

    for (int m = 0; m < qm.meshes.size(); m++)
    {
        qm.model.meshes[m] = qm.meshes[m];
        int matID = qm.materialIDs[m];
        qm.model.meshMaterial[m] = matID;
    }

    return qm;
}