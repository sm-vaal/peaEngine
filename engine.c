//
// Created by sergi on 21/12/2025.
//

#include "engine.h"
#include "rendering.h"
#include <math.h>
#include <stdlib.h>

Camera_t camera;
Mesh_t   mesh;
Model_t  model;

void initCamera() {
    camera.pitch     = 0.0f;
    camera.pitchRate = 0.02f;
    camera.yaw       = 0.0f;
    camera.yawRate   = 0.02f;

    camera.pos.x = 5;
    camera.pos.y = 5;
    camera.pos.z = -10;

    camera.vel.x = 0.2f;
    camera.vel.y = 0.2f;
    camera.vel.z = 0.2f;
}

void initMesh() {
    static const float s = 5;

    static Vec3_t points[] = {
        {0, 0, 0},  // bottom corners
        {s, 0, 0},
        {s, 0, s},
        {0, 0, s},

        {0, s, 0},  // top corners
        {s, s, 0},
        {s, s, s},
        {0, s, s},
    };

    static Face_t faces[] = {
        // bottom (y = 0)
        {0, 2, 1},
        {0, 3, 2},

        // top (y = s)
        {4, 5, 6},
        {4, 6, 7},

        // left (x = 0)
        {0, 7, 3},
        {0, 4, 7},

        // right (x = s)
        {1, 2, 6},
        {1, 6, 5},

        // front (z = 0)
        {0, 1, 5},
        {0, 5, 4},

        // back (z = s)
        {3, 7, 6},
        {3, 6, 2},

    };

    // a simple cube
    mesh = (Mesh_t) {
        .nFaces  = 12, // 6 faces, 2 triangles per face
        .nPoints = 8,  // 8 vertices
        .faces   = faces,
        .points  = points,
    };

    model = (Model_t) {
        mesh,
        calloc(mesh.nFaces, sizeof(Color))
    };

    model.faceColors[0]  = model.faceColors[1]  = RED;
    model.faceColors[2]  = model.faceColors[3]  = GREEN;
    model.faceColors[4]  = model.faceColors[5]  = BLUE;
    model.faceColors[6]  = model.faceColors[7]  = PURPLE;
    model.faceColors[8]  = model.faceColors[9]  = YELLOW;
    model.faceColors[10] = model.faceColors[11] = ORANGE;

}


// returns the 2d projection of the point
int projectVertex(Vec3_t vertex, Camera_t cam, Vec2Int_t* dest) {
    // view transform (world -> view)

    // translation relative to camera
    float x = vertex.x - cam.pos.x;
    float y = vertex.y - cam.pos.y;
    float z = vertex.z - cam.pos.z;

    // yaw rotation (Y axis)
    float cs = cosf(cam.yaw);
    float sn = sinf(cam.yaw);
    float xYaw = x * cs - z * sn;
    float zYaw = x * sn + z * cs;

    // pitch rotation (X axis)
    cs = cosf(cam.pitch);
    sn = sinf(cam.pitch);
    float yFinal = y * cs - zYaw * sn;
    float zFinal = y * sn + zYaw * cs;

    // clipping: don't project if behind camera
    if (zFinal <= 0.1f) return 0;

    // projection
    static const float fov = 200.0f;
    float xProjected = (xYaw/zFinal) * fov;
    float yProjected = (yFinal/zFinal) * fov;

    // centering at 0,0 and snap to grid
    dest->x = (int32_t) xProjected + (H_RES / 2);
    dest->y = (int32_t) yProjected + (V_RES / 2);
    return 1;
}

void projectAndDrawPts() {
    Vec2Int_t point;
    for (int i = 0; i < mesh.nPoints; i++) {
        if (projectVertex(mesh.points[i], camera, &point)) {
            plotPixel(point.x, point.y, WHITE);
        }
    }
}

void projectAndDrawWireframe() {
    // 3 points of a triangle
    Vec2Int_t projected[mesh.nPoints];
    int       valid[mesh.nPoints];

    for (int i = 0; i < mesh.nPoints; i++) {
        valid[i] = projectVertex(mesh.points[i], camera, &projected[i]); // project all points
    }

    for (int i = 0; i < mesh.nFaces; i++) {
        int32_t p1 = mesh.faces[i].a;
        int32_t p2 = mesh.faces[i].b;
        int32_t p3 = mesh.faces[i].c;
        if (valid[p1] && valid[p2]) {
            plotLine(projected[p1].x, projected[p1].y, projected[p2].x, projected[p2].y, WHITE);
        }
        if (valid[p2] && valid[p3]) {
            plotLine(projected[p2].x, projected[p2].y, projected[p3].x, projected[p3].y, WHITE);
        }
        if (valid[p3] && valid[p1]) {
            plotLine(projected[p3].x, projected[p3].y, projected[p1].x, projected[p1].y, WHITE);
        }
    }
}

void projectAndDrawWireframeBlocking() {
    // calculate all projections
    Vec2Int_t projected[model.mesh.nPoints];
    int valid[model.mesh.nPoints];

    for (int i = 0; i < model.mesh.nPoints; i++) {
        valid[i] = projectVertex(model.mesh.points[i], camera, &projected[i]); // project all points
    }

    Vec3_t normals[model.mesh.nFaces]; // not really needed per se, but in case they have to be used later

    for (int i = 0; i < model.mesh.nFaces; i++) {
        int32_t p1 = mesh.faces[i].a;
        int32_t p2 = mesh.faces[i].b;
        int32_t p3 = mesh.faces[i].c;

        normals[i] = normalize(
            calculateTriangleNormal(model.mesh.points[p1], model.mesh.points[p2], model.mesh.points[p3])
            );

        Vec3_t cam2Triangle = {
            model.mesh.points[p1].x - camera.pos.x,
            model.mesh.points[p1].y - camera.pos.y,
            model.mesh.points[p1].z - camera.pos.z
        };

        cam2Triangle = normalize(cam2Triangle);

        Color faceColor = model.faceColors[i];

        if (dotProduct(cam2Triangle, normals[i]) > 0.0f) {
            if (valid[p1] && valid[p2]) {
                plotLine(projected[p1].x, projected[p1].y, projected[p2].x, projected[p2].y, faceColor);
            }
            if (valid[p2] && valid[p3]) {
                plotLine(projected[p2].x, projected[p2].y, projected[p3].x, projected[p3].y, faceColor);
            }
            if (valid[p3] && valid[p1]) {
                plotLine(projected[p3].x, projected[p3].y, projected[p1].x, projected[p1].y, faceColor);
            }
        }
    }


}

void projectAndDrawModel() {
    Vec2Int_t projected[model.mesh.nPoints];
    int valid[model.mesh.nPoints];

    for (int i = 0; i < model.mesh.nPoints; i++) {
        valid[i] = projectVertex(model.mesh.points[i], camera, &projected[i]);
    }

    Vec3_t normals[model.mesh.nFaces];

    for (int i = 0; i < model.mesh.nFaces; i++) {
        int32_t p1 = mesh.faces[i].a;
        int32_t p2 = mesh.faces[i].b;
        int32_t p3 = mesh.faces[i].c;

        normals[i] = normalize(
            calculateTriangleNormal(
                model.mesh.points[p1],
                model.mesh.points[p2],
                model.mesh.points[p3])
            );

        Vec3_t cam2Triangle = {
            model.mesh.points[p1].x - camera.pos.x,
            model.mesh.points[p1].y - camera.pos.y,
            model.mesh.points[p1].z - camera.pos.z
        };

        float dist = vec3Length(cam2Triangle);
        uint8_t shade;
        float distRatio = dist / 30.0f;

        if (distRatio >= 1) {
            shade = 0;
        } else {
            shade = 255 - ((uint8_t) (255.0f * (dist / 30.0f)));
        }

        cam2Triangle = normalize(cam2Triangle);

        Color faceColor = model.faceColors[i];

        faceColor.r = (faceColor.r * shade) / 255;
        faceColor.g = (faceColor.g * shade) / 255;
        faceColor.b = (faceColor.b * shade) / 255;

        if (dotProduct(cam2Triangle, normals[i]) > 0.0f) {
            if (valid[p1] && valid[p2] && valid[p3]) {
                plotTriangle(projected[p1], projected[p2], projected[p3], faceColor);
            }
        }
    }


}



void pollInput() {
    if (IsKeyDown(KEY_W)) {
        camera.pos.z += camera.vel.z * cos(camera.yaw);
        camera.pos.x += camera.vel.x * sin(camera.yaw);
    }
    if (IsKeyDown(KEY_S)) {
        camera.pos.z -= camera.vel.z * cos(camera.yaw);
        camera.pos.x -= camera.vel.x * sin(camera.yaw);
    }
    if (IsKeyDown(KEY_A)) {
        camera.pos.x -= camera.vel.x * cos(camera.yaw);
        camera.pos.z += camera.vel.z * sin(camera.yaw);
    }
    if (IsKeyDown(KEY_D)) {
        camera.pos.x += camera.vel.x * cos(camera.yaw);
        camera.pos.z -= camera.vel.z * sin(camera.yaw);
    }
    if (IsKeyDown(KEY_Q)) {
        camera.pos.y -= camera.vel.y;
    }
    if (IsKeyDown(KEY_E)) {
        camera.pos.y += camera.vel.y;
    }
    if (IsKeyDown(KEY_UP)) {
        camera.pitch -= camera.pitchRate;
    }
    if (IsKeyDown(KEY_DOWN)) {
        camera.pitch += camera.pitchRate;
    }
    if (IsKeyDown(KEY_LEFT)) {
        camera.yaw -= camera.yawRate;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        camera.yaw += camera.yawRate;
    }
}

Vec3_t calculateTriangleNormal(Vec3_t v1, Vec3_t v2, Vec3_t v3) {
    // two sides of the triangle
    Vec3_t a = {
        v2.x - v1.x,
        v2.y - v1.y,
        v2.z - v1.z
    };

    Vec3_t b = {
        v3.x - v1.x,
        v3.y - v1.y,
        v3.z - v1.z
    };

    // cross product
    Vec3_t normal = {
        a.y * b.z - a.z * b.y, // i
        a.z * b.x - a.x * b.z, // j
        a.x * b.y - a.y * b.x  // k
    };

    return normal;
}

// to compare camera vector to triangle normal
Vec3_t getCameraVector() {
    Vec3_t v = {
        cosf(camera.pitch) * sinf(camera.yaw),// x
        sinf(camera.pitch),                   // y
        cosf(camera.pitch) * cosf(camera.yaw) // z
    };
    return v;
}

Vec3_t normalize(Vec3_t v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len > 0.0f) {
        v.x /= len;
        v.y /= len;
        v.z /= len;
    }
    return v;
}

float dotProduct(Vec3_t v1, Vec3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float vec3Length(Vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}