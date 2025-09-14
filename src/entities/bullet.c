#include "bullet.h"
#include "raymath.h"

// Modular bullet functions
void bullet_init(Bullet* bullets, int maxBullets) {
    for (int i = 0; i < maxBullets; ++i) {
        bullets[i].active = false;
    }
}

// Spawns a Bullet
void bullet_spawn(Bullet* bullets, int maxBullets, Vector3 StartPos, Vector3 Dir, float speed) {
    float length = Vector3Length(Dir);
    if (length == 0)
        return;
    Dir = Vector3Divide(Dir, (Vector3){length, length, length});

    for (int i = 0; i < maxBullets; i++) {
        if (!bullets[i].active) {
            bullets[i].Pos = StartPos;
            bullets[i].Vel = Vector3Multiply(Dir, (Vector3){speed, speed, speed});
            bullets[i].life = 2.0f;
            bullets[i].active = true;
            break;
        }
    }
}

// Update Bullets
void bullet_update(Bullet* bullets, int maxBullets) {
    double deltaTime = GetFrameTime();

    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].active) {
            bullets[i].Pos = Vector3Add(bullets[i].Pos, Vector3Scale(bullets[i].Vel, deltaTime));

            bullets[i].life -= deltaTime;
            if (bullets[i].life <= 0) {
                bullets[i].active = false;
            }
        }
    }
}