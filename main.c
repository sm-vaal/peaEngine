#include <stdio.h>

#include "engine.h"
#include "raylib.h"
#include "rendering.h"
#include "obj_loader.h"
#include "string.h"

int main(int argc, char** argv) {

    SetTraceLogLevel(LOG_NONE);
    InitWindow(H_RES * SCALE, V_RES * SCALE, "peaEngine3d");
    SetTargetFPS(60);

    if (argc == 1) {
        modelLoaded = false;
        printf("Usage: peaEngine [model.obj] [-a] (-a for antialiasing)\n");
    }
    else if (argc == 2) {
        fileName = argv[1];
        printf("loading model %s\n", fileName);
        modelLoaded = true;
    }
    else if (argc == 3) {
        fileName = argv[1];
        printf("loading model %s\n", fileName);
        modelLoaded = true;
        if (strcmp(argv[2], "-a") == 0) {
            printf("anti-aliasing on: low performance likely\n");
            antiAliasingOn = true;
        }
    }

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
