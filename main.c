#include <stdio.h>

#include "engine.h"
#include "raylib.h"
#include "rendering.h"

int main(void) {

    SetTraceLogLevel(LOG_NONE);
    InitWindow(H_RES * SCALE, V_RES * SCALE, "peaEngine3d");
    SetTargetFPS(60);
    initFrameBuffer();

    initCamera();
    initMesh();

    while (!WindowShouldClose()) {
        pollInput();
        updateFrameBuffer();
        BeginDrawing();
        plotFrameBuffer();
        DrawFPS(5, 5);
        EndDrawing();
    }

    freeFrameBuffer();

    return 0;
}
