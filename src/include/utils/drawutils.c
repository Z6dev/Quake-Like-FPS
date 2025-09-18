#include "drawutils.h"
#include "raylib.h"
#include "rlgl.h"

// Helper: emit one quad face (normal + 4 texcoord/vertex pairs)
static void EmitFace(const Vector3 verts[], const int idx[4], const Vector2 uvs[4], Vector3 normal, Color color) {
    rlNormal3f(normal.x, normal.y, normal.z);
    for (int j = 0; j < 4; j++) {
        rlTexCoord2f(uvs[j].x, uvs[j].y);
        int vi = idx[j];
        rlVertex3f(verts[vi].x, verts[vi].y, verts[vi].z);
    }
}

//------------------------------------------------------------------------------------
// Custom Functions Definition
//------------------------------------------------------------------------------------
// Draw cube textured
// NOTE: Cube position is the center position
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length,
                     Color color) {
    float x = position.x, y = position.y, z = position.z;
    float hx = width / 2, hy = height / 2, hz = length / 2;

    // 8 cube corners
    Vector3 v[8] = {
        {x - hx, y - hy, z - hz}, // 0
        {x + hx, y - hy, z - hz}, // 1
        {x + hx, y + hy, z - hz}, // 2
        {x - hx, y + hy, z - hz}, // 3
        {x - hx, y - hy, z + hz}, // 4
        {x + hx, y - hy, z + hz}, // 5
        {x + hx, y + hy, z + hz}, // 6
        {x - hx, y + hy, z + hz}  // 7
    };

    // Faces: each is 4 indices into v[]
    const int faces[6][4] = {
        {4, 5, 6, 7}, // Front
        {0, 3, 2, 1}, // Back
        {3, 7, 6, 2}, // Top
        {0, 1, 5, 4}, // Bottom
        {1, 2, 6, 5}, // Right
        {0, 4, 7, 3}  // Left
    };

    // Normals per face
    const Vector3 normals[6] = {
        {0.0f, 0.0f, 1.0f},  // Front
        {0.0f, 0.0f, -1.0f}, // Back
        {0.0f, 1.0f, 0.0f},  // Top
        {0.0f, -1.0f, 0.0f}, // Bottom
        {1.0f, 0.0f, 0.0f},  // Right
        {-1.0f, 0.0f, 0.0f}  // Left
    };

    // UVs per face (0..1) matching the original orientation
    const Vector2 faceUVs[6][4] = {
        {{0,1},{1,1},{1,0},{0,0}}, // Front
        {{1,1},{1,0},{0,0},{0,1}}, // Back
        {{0,0},{0,1},{1,1},{1,0}}, // Top
        {{1,0},{0,0},{0,1},{1,1}}, // Bottom
        {{1,1},{1,0},{0,0},{0,1}}, // Right
        {{0,1},{1,1},{1,0},{0,0}}  // Left
    };

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    for (int i = 0; i < 6; i++) {
        EmitFace(v, faces[i], (Vector2 *)faceUVs[i], normals[i], color);
    }

    rlEnd();
    rlSetTexture(0);
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width,
                        float height, float length, Color color) {
    float x = position.x, y = position.y, z = position.z;
    float hx = width / 2, hy = height / 2, hz = length / 2;
    float texW = (float)texture.width, texH = (float)texture.height;

    Vector3 v[8] = {
        {x - hx, y - hy, z - hz}, {x + hx, y - hy, z - hz}, {x + hx, y + hy, z - hz}, {x - hx, y + hy, z - hz},
        {x - hx, y - hy, z + hz}, {x + hx, y - hy, z + hz}, {x + hx, y + hy, z + hz}, {x - hx, y + hy, z + hz}
    };

    const int faces[6][4] = {
        {4, 5, 6, 7}, {0, 3, 2, 1}, {3, 7, 6, 2}, {0, 1, 5, 4}, {1, 2, 6, 5}, {0, 4, 7, 3}
    };

    const Vector3 normals[6] = {
        {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}
    };

    // compute normalized source coordinates
    float sx = source.x / texW, sy = source.y / texH;
    float sw = source.width / texW, sh = source.height / texH;

    // UVs per face (using same relative ordering as original)
    Vector2 faceUVs[6][4] = {
        {{sx, sy + sh}, {sx + sw, sy + sh}, {sx + sw, sy}, {sx, sy}},           // Front
        {{sx + sw, sy + sh}, {sx + sw, sy}, {sx, sy}, {sx, sy + sh}},           // Back
        {{sx, sy}, {sx, sy + sh}, {sx + sw, sy + sh}, {sx + sw, sy}},           // Top
        {{sx + sw, sy}, {sx, sy}, {sx, sy + sh}, {sx + sw, sy + sh}},           // Bottom
        {{sx + sw, sy + sh}, {sx + sw, sy}, {sx, sy}, {sx, sy + sh}},           // Right
        {{sx, sy + sh}, {sx + sw, sy + sh}, {sx + sw, sy}, {sx, sy}}            // Left
    };

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    for (int i = 0; i < 6; i++) {
        EmitFace(v, faces[i], faceUVs[i], normals[i], color);
    }

    rlEnd();
    rlSetTexture(0);
}