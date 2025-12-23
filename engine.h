//
// Created by sergi on 21/12/2025.
//

#ifndef PEAENGINE3D_ENGINE_H
#define PEAENGINE3D_ENGINE_H

#include <stdint.h>
#include "raylib.h"

typedef struct {
    float x, y, z;
} Vec3_t;

typedef struct {
    int32_t x, y;
} Vec2Int_t;

typedef struct {
    int32_t a, b, c; // connections between 3 points, triangles
} Face_t;

typedef struct {
    Vec3_t* points;   // array of vertices
    int32_t nPoints;
    Face_t* faces;    // array of connections
    int32_t nFaces;
} Mesh_t; // a 3d object is just triangles

typedef struct {
    Mesh_t mesh;
    Color* faceColors; // just a mesh with colors per face
} Model_t;

typedef struct {
    Vec3_t pos;
    Vec3_t vel; // increments per frame, for movement
    float  yaw;       // side to side, in
    float  yawRate;
    float  pitch;     // up and down
    float  pitchRate;
} Camera_t;

void initCamera();
void initMesh();
int projectVertex(Vec3_t vertex, Camera_t cam, Vec2Int_t* dest);
Vec3_t calculateTriangleNormal(Vec3_t v1, Vec3_t v2, Vec3_t v3);
void projectAndDrawPts();
void projectAndDrawWireframe();
void projectAndDrawModel();
void projectAndDrawModelWithMesh();
void pollInput();

Vec3_t getCameraVector();

Vec3_t normalize(Vec3_t v);
float dotProduct(Vec3_t v1, Vec3_t v2);
float vec3Length(Vec3_t v);


#endif //PEAENGINE3D_ENGINE_H