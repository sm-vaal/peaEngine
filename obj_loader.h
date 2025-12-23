//
// Created by sergi on 23/12/2025.
//

#ifndef PEAENGINE_OBJ_LOADER_H
#define PEAENGINE_OBJ_LOADER_H

#include "engine.h"

extern const char* fileName;
extern bool modelLoaded;

int loadObjToMesh(Mesh_t* outMesh, Camera_t* cam, float scale, const char* filename);

#endif //PEAENGINE_OBJ_LOADER_H