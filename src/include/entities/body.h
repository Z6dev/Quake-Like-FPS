#ifndef BODY_H
#define BODY_H

#include "raylib.h"

#define GRAVITY 32.0f
#define CROUCH_SPEED 5.0f
#define JUMP_FORCE 12.0f
#define MAX_ACCEL 150.0f
// Grounded drag
#define FRICTION 0.86f
// Increasing air drag, increases strafing speed
#define AIR_DRAG 0.98f
// Responsiveness for turning movement direction to looked direction
#define CONTROL 15.0f
// Defined for the step height, self explanatory
#define STEP_HEIGHT 0.6f

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

    int health;
} Player;

// Obstacle Structure
typedef struct {
    Vector3 position;
    Vector3 size;
} Obstacle;

void UpdatePlayerBody(Body* body, float rot, char side, char forward, bool jumpPressed,
                      bool crouchHold, Obstacle* obstacles, int elem_num);
BoundingBox GetBoundingBox(Vector3 position, Vector3 size);

void obstacles_init(Obstacle* obstacles);
#endif