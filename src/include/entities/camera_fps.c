#include "camera_fps.h"
#include "raylib.h"
#include "raymath.h"

// Update camera for FPS behaviour
void UpdateCameraFPS(Camera3D* camera, Player* player) {
    const Vector3 up = (Vector3){0.0f, 1.0f, 0.0f};
    const Vector3 targetOffset = (Vector3){0.0f, 0.0f, -1.0f};

    // Left and right
    Vector3 yaw = Vector3RotateByAxisAngle(targetOffset, up, player->lookRotation.x);

    // Clamp view up
    float maxAngleUp = Vector3Angle(up, yaw);
    maxAngleUp -= 0.001f; // Avoid numerical errors
    if (-(player->lookRotation.y) > maxAngleUp) {
        player->lookRotation.y = -maxAngleUp;
    }

    // Clamp view down
    float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
    maxAngleDown *= -1.0f;  // Downwards angle is negative
    maxAngleDown += 0.001f; // Avoid numerical errors
    if (-(player->lookRotation.y) < maxAngleDown) {
        player->lookRotation.y = -maxAngleDown;
    }

    // Up and down
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    // Rotate view vector around right axis
    float pitchAngle = -player->lookRotation.y - player->lean.y;
    pitchAngle =
        Clamp(pitchAngle, -PI / 2 + 0.0001f,
              PI / 2 - 0.0001f); // Clamp angle so it doesn't go past straight up or straight down
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

    // Head animation
    // Rotate up direction around forward axis
    float headSin = sin(player->headTimer * PI);
    float headCos = cos(player->headTimer * PI);
    const float stepRotation = 0.01f;
    camera->up = Vector3RotateByAxisAngle(up, pitch, headSin * stepRotation + player->lean.x);

    // Camera BOB
    const float bobSide = 0.1f;
    const float bobUp = 0.15f;
    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, player->walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}