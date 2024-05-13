#include <iostream>
#include "map/map.h"
#include <raylib.h>

using std::cout;
using std::endl;

void banner()
{
    cout << "QFORMAT test app\n"
         << "----------------"
         << endl;
}

int main()
{

    banner();
    auto map = qformats::map::QMap::FromFIle("../testdata/qmap_standard.map");
    cout << "map version: " << map->MapData()->Version() << endl;

    InitWindow(800, 600, "raylib game template");
    SetTargetFPS(60);

    Mesh mesh = {0};

    auto b = map->BrushEntities()[0]->brushes[0];
    mesh.triangleCount = b->vertCount / 3;
    mesh.vertexCount = b->vertCount;
    // mesh.indices = (ushort *)MemAlloc(b->indiceCount * sizeof(ushort));
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));  // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float)); // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));   // 3 vertices, 3 coordinates each (x, y, z)

    int i = 0;
    int i_indices = 0;
    int inverseScale = 24;

    for (auto p : b->polys)
    {
        for (auto v : p.vertices)
        {
            mesh.vertices[i] = v.x / inverseScale;
            mesh.normals[i] = p.face.planeNormal.x;
            i++;
            mesh.vertices[i] = v.z / inverseScale;
            mesh.normals[i] = p.face.planeNormal.z;
            i++;
            mesh.vertices[i] = v.y / inverseScale;
            mesh.normals[i] = p.face.planeNormal.y;
            i++;
            std::cout << std::printf("normal (%f %f %f)", p.face.planeNormal.x, p.face.planeNormal.y, p.face.planeNormal.z) << std::endl;
        }

        /*  for (auto idx : p.indices)
          {
              cout << "add indice: " << idx << endl;
              mesh.indices[i_indices] = idx;
              i_indices++;
          }
          */
    }

    UploadMesh(&mesh, false);
    auto m = LoadModelFromMesh(mesh);
    auto cube = LoadModelFromMesh(GenMeshCube(2.0f, 1.0f, 2.0f));

    // Define the camera to look into our 3d world
    Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0};
    Vector3 position = {0.0f, 0.0f, 0.0f};
    auto c = Color();
    c.r = 255;
    c.g = 10;
    c.b = 10;

    DisableCursor();
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            EnableCursor();
        }
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(m, position, 1, DARKBLUE);
        DrawModelWires(m, position, 1, BLACK);
        DrawGrid(10, 1.0);

        EndMode3D();
        EndDrawing();
    }
    return 0;
}
