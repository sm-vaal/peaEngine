//
// Created by sergi on 23/12/2025.
//

#include "obj_loader.h"
#include <stdlib.h>
#include <stdio.h>

const char* fileName;
bool modelLoaded = false;

// assumes the obj file is valid and correct
// very rudimentary parsing, invalid files WILL break stuff
int loadObjToMesh(Mesh_t* outMesh, Camera_t* cam, float scale, const char* filename) {
    FILE* fptr = fopen(filename, "r");
    if (!fptr) { return -1; }

    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);
    if (size <= 0) return -1;

    char* fileContents = (char*) malloc(size + 1);
    if (!fileContents) { return -1; }

    fread(fileContents, 1, size, fptr);
    fclose(fptr);

    fileContents[size] = '\0'; // null terminator

    int nVertices = 0;
    int nFaces = 0;

    // count vertices and faces
    for (int i = 0; i < size; i++) {
        if (fileContents[i] == 'v' && fileContents[i + 1] == ' ') {
            nVertices++;
        }
        if (fileContents[i] == 'f' && fileContents[i + 1] == ' ') {
            nFaces++;
        }
    }

    outMesh->nFaces  = nFaces;
    outMesh->nPoints = nVertices;

    outMesh->faces   = malloc(nFaces * sizeof(Face_t));
    if (!outMesh->faces) {return -1;}

    outMesh->points  = malloc(nVertices * sizeof(Vec3_t));
    if (!outMesh->points) {return -1;}


    char* data = fileContents;
    int currVertex = 0;
    int currFace = 0;

    while (*data != '\0') {
        if (*data == 'v') {
            float x = strtof(data + 1, &data);
            float y = strtof(data + 1, &data);
            float z = strtof(data + 1, &data);
            Vec3_t point = {x * scale, y * scale, z * scale};
            outMesh->points[currVertex] = point;
            currVertex++;
        }
        if (*data == 'f') {
            int32_t p1 = strtol(data + 1, &data, 10);
            int32_t p2 = strtol(data + 1, &data, 10);
            int32_t p3 = strtol(data + 1, &data, 10);
            Face_t face = {p1-1, p2-1, p3-1};
            outMesh->faces[currFace] = face;
            currFace++;
        }
        else data++;
    }

    free(fileContents);
    return 0;
}