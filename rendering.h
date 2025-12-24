//
// Created by sergi on 21/12/2025.
//

#ifndef PEAENGINE3D_RENDERING_H
#define PEAENGINE3D_RENDERING_H

#include "raylib.h"
#include <stdint.h>
#include "engine.h"

#define H_RES 1280
#define V_RES 720
#define SCALE 1

extern Color* frameBuffer;
extern bool   antiAliasingOn;
extern int    renderMode;

void initFrameBuffer();
void freeFrameBuffer();

void plotFrameBuffer();
void updateFrameBuffer();

void plotTriangle(Vec2Int_t v0, Vec2Int_t v1, Vec2Int_t v2, Color color);
void plotLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color);
void plotPixel(int32_t x, int32_t y, Color color);

void swap(int32_t *a, int32_t *b);
void swapVec2Int(Vec2Int_t* a, Vec2Int_t* b);

void antiAliasing();

#endif //PEAENGINE3D_RENDERING_H