//
// Created by sergi on 24/12/2025.
//

#ifndef PEAENGINE_COMMAND_PARSE_H
#define PEAENGINE_COMMAND_PARSE_H

#include <stdbool.h>
#include "rendering.h"

typedef struct {
    const char* filename;
    bool aliasing;
    int renderType;
} parsedCommand_t;

typedef enum {
    POINTS,
    WIREFRAME,
    MODEL_ONLY,
    MODEL_MESH,
    MODEL_SHADING,
} renderType_t;

parsedCommand_t parseCommand(int argc, char ** argv);

#endif //PEAENGINE_COMMAND_PARSE_H