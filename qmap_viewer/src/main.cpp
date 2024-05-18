#include <iostream>
#include <map/map.h>
#include <raylib.h>
#include <raymath.h>
#include <filesystem>

#include <common/textureman.h>
#include "QuakeMap.h"
#include <map>

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
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 600, "raypowered Quake Map Viewer");
    SetTargetFPS(60);

    auto qm = QuakeMap(QuakeMapOptions{
        texturePath : "testdata/textures/"
    });
    qm.LoadMapFromFile("testdata/qmap_valve.map");

    // Define the camera to look into our 3d world
    Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0};

    auto c = Color();
    c.r = 255;
    c.g = 10;
    c.b = 10;

    DisableCursor();
    bool showWireframe = false;
    Color wireColor{255, 100, 100, 255};
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyPressed(KEY_M))
        {
            EnableCursor();
        }

        if (IsKeyPressed(KEY_F))
        {
            qm.renderMode = qm.renderMode == QM_TEXTURED ? QM_WIRERFAME : QM_TEXTURED;
            qm.ChangeRenderMode(qm.renderMode);
        }

        if (IsCursorHidden())
        {
            UpdateCamera(&camera, CAMERA_FREE);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        qm.DrawQuakeSolids();

        EndMode3D();
        EndDrawing();
    }

    return 0;
}
