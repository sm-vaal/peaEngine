//
// Created by sergi on 24/12/2025.
//

#include "command_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

parsedCommand_t parseCommand(int argc, char ** argv) {

    parsedCommand_t result;
    result.filename = NULL;             // Default: NULL
    result.aliasing = false;            // Default: NOT -a
    result.renderType = MODEL_SHADING;  // Default: -ms

    for (int i = 1; i < argc; i++) {
        char* currentArg = argv[i];

        // Check if the argument is a flag (starts with '-')
        if (currentArg[0] == '-') {
            // -- Render Modes --
            if (strcmp(currentArg, "-p") == 0) {
                result.renderType = POINTS;
            }
            else if (strcmp(currentArg, "-w") == 0) {
                result.renderType = WIREFRAME;
            }
            else if (strcmp(currentArg, "-m") == 0) {
                result.renderType = MODEL_ONLY;
            }
            else if (strcmp(currentArg, "-mm") == 0) {
                result.renderType = MODEL_MESH;
            }
            else if (strcmp(currentArg, "-ms") == 0) {
                result.renderType = MODEL_SHADING;
            }
            // -- Antialiasing --
            else if (strcmp(currentArg, "-a") == 0) {
                result.aliasing = true;
            }
        }
        else {
            // If it doesn't start with '-', and we haven't found a filename yet,
            // we assume this is the filename argument.
            if (result.filename == NULL) {
                result.filename = currentArg;
            }
        }
    }

    return result;
}

