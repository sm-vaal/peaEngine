//
// Created by sergi on 21/12/2025.
//

#include "rendering.h"

#include <stdlib.h>
#include <string.h>

Color* frameBuffer;
Color* antiAliasingFrameBuffer = NULL;
Texture2D fbTexture;
Image fbImage;

bool antiAliasingOn = false;

void initFrameBuffer() {
    frameBuffer = (Color*) calloc( H_RES * V_RES, sizeof(Color));

    fbImage = (Image) {
        .data = frameBuffer,
        .width = H_RES,
        .height = V_RES,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    if (antiAliasingOn) {
        antiAliasingFrameBuffer = (Color*) calloc( H_RES * V_RES, sizeof(Color));
        fbImage.data = antiAliasingFrameBuffer;
    }

    fbTexture = LoadTextureFromImage(fbImage);
}

void updateFrameBuffer() {
    ClearBackground(BLACK);
    memset(frameBuffer, 0, H_RES * V_RES * sizeof(Color));

    projectAndDrawModelWithMesh();

    if (!antiAliasingOn) {
        UpdateTexture(fbTexture, frameBuffer);
    } else {
        antiAliasing();
        UpdateTexture(fbTexture, antiAliasingFrameBuffer);
    }

}

void plotFrameBuffer() {
    static const Rectangle src = {0, 0, H_RES, V_RES};
    static const Rectangle dst = {0, 0, H_RES * SCALE, V_RES * SCALE};

    DrawTexturePro(fbTexture, src, dst, (Vector2) {0, 0}, 0.0f, WHITE);
}

void plotLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, Color color) {
    // uses Bresenham's algorithm to plot

    if (x0 < 0 || x0 >= H_RES ||
        y0 < 0 || y0 >= V_RES ||
        x1 < 0 || x1 >= H_RES ||
        y1 < 0 || y1 >= V_RES) {
        return;
    }

    int32_t dx = abs(x1 - x0);
    int32_t dy = abs(y1 - y0);

    int32_t sx = (x0 < x1) ? 1 : -1; // swap x
    int32_t sy = (y0 < y1) ? 1 : -1; // swap y
    int32_t err = dx - dy;

    while (1) {
        frameBuffer[x0 + y0 * H_RES] = color;

        if (x0 == x1 && y0 == y1) break;

        int32_t e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void plotPixel(int32_t x, int32_t y, Color color) {
    if (x < 0 || x >= H_RES || y < 0 || y >= V_RES) return;
    frameBuffer[x + y * H_RES] = color;
}

void plotTriangle(Vec2Int_t v0, Vec2Int_t v1, Vec2Int_t v2, Color color) {
    // sort by Y to plot top to bottom
    if (v0.y > v1.y) swapVec2Int(&v0, &v1);
    if (v0.y > v2.y) swapVec2Int(&v0, &v2);
    if (v1.y > v2.y) swapVec2Int(&v1, &v2);

    int32_t totalHeight = v2.y - v0.y;
    if (totalHeight == 0) return; // flat line triangle

    for (int32_t i = 0; i < totalHeight; i++) {
        int32_t y = v0.y + i;

        if (y < 0 || y >= V_RES) continue;

        int32_t segmentHeight = v1.y - v0.y + 1; // +1 to avoid / by 0
        bool second_half = i > v1.y - v0.y || v1.y == v0.y;

        int32_t alpha_num = i;
        int32_t alpha_den = totalHeight;

        int32_t beta_num  = second_half ? (i - (v1.y - v0.y)) : i;
        int32_t beta_den  = second_half ? (v2.y - v1.y) : (v1.y - v0.y);

        if (beta_den == 0) beta_den = 1;

        int32_t A_x = v0.x + ((v2.x - v0.x) * alpha_num) / alpha_den;
        int32_t B_x;
        if (second_half) {
            B_x = v1.x + ((v2.x - v1.x) * beta_num) / beta_den;
        } else {
            B_x = v0.x + ((v1.x - v0.x) * beta_num) / beta_den;
        }

        // Sort A and B so A is always left
        if (A_x > B_x) swap(&A_x, &B_x);

        // Draw Scanline
        if (A_x < 0) A_x = 0;
        if (B_x >= H_RES) B_x = H_RES - 1;

        for (int32_t x = A_x; x <= B_x; x++) {
            frameBuffer[x + y * H_RES] = color;
        }
    }
}

void freeFrameBuffer() {
    free(frameBuffer);
    if (antiAliasingFrameBuffer != NULL) {
        free(antiAliasingFrameBuffer);
    }
}

void swap(int32_t *a, int32_t *b) {
    int32_t t = *a;
    *a = *b;
    *b = t;
}

void swapVec2Int(Vec2Int_t* a, Vec2Int_t* b) {
    Vec2Int_t t = *a;
    *a = *b;
    *b = t;
}

// vaseline filter, gaussian blur. KILLS performance
void antiAliasing() {
    const static int weights[3][3] = {
        {1, 1, 1},
        {1, 10, 1},
        {1, 1, 1}
    };

    const static int totalWeight = 18;

    // Handle borders: just copy them (no blur)
    memcpy(antiAliasingFrameBuffer, frameBuffer,
           H_RES * V_RES * sizeof(Color));

    for (int y = 1; y < V_RES - 1; y++) {
        for (int x = 1; x < H_RES - 1; x++) {

            int R = 0;
            int G = 0;
            int B = 0;

            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {

                    int px = x + kx - 1;
                    int py = y + ky - 1;

                    Color p = frameBuffer[px + py * H_RES];
                    int w = weights[ky][kx];

                    R += p.r * w;
                    G += p.g * w;
                    B += p.b * w;
                }
            }

            Color out;
            out.r = R / totalWeight;
            out.g = G / totalWeight;
            out.b = B / totalWeight;
            out.a = 255;

            antiAliasingFrameBuffer[x + y * H_RES] = out;
        }
    }
}