#include "body.h"
#include "raylib.h"
#include "raymath.h"

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


// Update body considering current world state
void UpdatePlayerBody(Body* body, float rot, char side, char forward, bool jumpPressed,
                      bool crouchHold, Obstacle* obstacles, int elem_num) {
    body->prevPos = body->position;
    body->justJumped = false;

    Vector2 input = (Vector2){(float)side, (float)-forward};
    float delta = GetFrameTime();

    // Gravity
    if (!body->isGrounded)
        body->velocity.y -= GRAVITY * delta;

    // Jump
    if (body->isGrounded && jumpPressed) {
        body->velocity.y = JUMP_FORCE;
        body->isGrounded = false;
        body->justJumped = true;
    }

    // Calculate movement direction
    Vector3 front = (Vector3){sinf(rot), 0.f, cosf(rot)};
    Vector3 right = (Vector3){cosf(-rot), 0.f, sinf(-rot)};
    Vector3 desiredDir = {
        input.x * right.x + input.y * front.x,
        0.0f,
        input.x * right.z + input.y * front.z,
    };
    body->dir = Vector3Lerp(body->dir, desiredDir, CONTROL * delta);

    // Friction / air drag
    float decel = (body->isGrounded ? FRICTION : AIR_DRAG);
    Vector3 hvel = {body->velocity.x * decel, 0.0f, body->velocity.z * decel};

    if (Vector3Length(hvel) < (body->MoveSpeed * 0.01f))
        hvel = (Vector3){0};

    float speed = Vector3DotProduct(hvel, body->dir);
    float maxSpeed = (crouchHold ? CROUCH_SPEED : body->MoveSpeed);
    float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * delta);
    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    // Try move along X
    body->position.x += body->velocity.x * delta;
    BoundingBox playerBox = GetBoundingBox(body->position, (Vector3){1.0f, 2.0f, 1.0f});
    for (int i = 0; i < elem_num; i++) {
        BoundingBox obsBox = GetBoundingBox(obstacles[i].position, obstacles[i].size);
        if (CheckCollisionBoxes(playerBox, obsBox)) {
            body->position.x = body->prevPos.x; // cancel X move
            break;
        }
    }

    // Try move along Z
    body->position.z += body->velocity.z * delta;
    playerBox = GetBoundingBox(body->position, (Vector3){1.0f, 2.0f, 1.0f});
    for (int i = 0; i < elem_num; i++) {
        BoundingBox obsBox = GetBoundingBox(obstacles[i].position, obstacles[i].size);
        if (CheckCollisionBoxes(playerBox, obsBox)) {
            body->position.z = body->prevPos.z; // cancel Z move
            break;
        }
    }

    // Apply vertical move (Y)
    body->position.y += body->velocity.y * delta;
    playerBox = GetBoundingBox(body->position, (Vector3){1.0f, 2.0f, 1.0f});
    for (int i = 0; i < elem_num; i++) {
        BoundingBox obsBox = GetBoundingBox(obstacles[i].position, obstacles[i].size);
        if (CheckCollisionBoxes(playerBox, obsBox)) {
            body->position.y = body->prevPos.y; // cancel Z move
            body->isGrounded = true;
            break;
        }
    }

    if (body->position.y <= 0.0f) {
        body->position.y = 0.0f;
        body->velocity.y = 0.0f;
        body->isGrounded = true;
    }
}

BoundingBox GetBoundingBox(Vector3 position, Vector3 size) {
    Vector3 truePosition = Vector3Subtract(position, (Vector3){size.x / 2.0f, 0.0f, size.z / 2.0f});
    BoundingBox hitbox = {truePosition, Vector3Add(truePosition, size)};

    return hitbox;
}