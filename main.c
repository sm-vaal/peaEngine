#include <stdio.h>

#include "engine.h"
#include "raylib.h"
#include "rendering.h"
#include "obj_loader.h"
#include "string.h"
#include "command_parse.h"

int main(int argc, char** argv) {

    SetTraceLogLevel(LOG_NONE);
    InitWindow(H_RES * SCALE, V_RES * SCALE, "peaEngine3d");
    SetTargetFPS(60);

    if (argc == 1) {
        printf("Usage: peaEngine [model.obj] [-a] (-a for antialiasing)\n");
    }

    parsedCommand_t parsedCommand = parseCommand(argc, argv);

    modelLoaded    = parsedCommand.filename != NULL;
    fileName       = parsedCommand.filename;
    if (fileName != NULL) {
        printf("loading model %s\n", fileName);
    }
    antiAliasingOn = parsedCommand.aliasing;
    if (antiAliasingOn) {
        printf("anti-aliasing on: low performance likely\n");
    }
    renderMode = parsedCommand.renderType;

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
