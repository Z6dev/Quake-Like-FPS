#ifndef BODY_H
#define BODY_H

#include "raylib.h"

// Body structure
typedef struct {
    Vector3 position;
    Vector3 prevPos;
    Vector3 velocity;
    Vector3 dir;

    bool isGrounded;
    bool justJumped;
    float MoveSpeed;
} Body;

// Player Structure
typedef struct {
    Body body;
    Vector2 lookRotation;
    float headTimer, walkLerp, headLerp;
    Vector2 lean;
} Player;

// Obstacle Structure
typedef struct {
    Vector3 position;
    Vector3 size;
} Obstacle;

void UpdatePlayerBody(Body* body, float rot, char side, char forward, bool jumpPressed,
                      bool crouchHold, Obstacle* obstacles, int elem_num);
BoundingBox GetBoundingBox(Vector3 position, Vector3 size);

// Player bounding box size: width=1.0f, height=2.0f, depth=1.0f

#endif