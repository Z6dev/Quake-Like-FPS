#include "bullet.h"
#include "body.h"
#include "raylib.h"
#include "raymath.h"

// Modular bullet functions
void bullet_init(Bullet* bullets, int maxBullets) {
    for (int i = 0; i < maxBullets; ++i) {
        bullets[i].active = false;
    }
}

// Spawns a Bullet
void bullet_spawn(Bullet* bullets, int maxBullets, Vector3 StartPos, Vector3 Dir, float speed, float size) {
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
            bullets[i].size = 0.7f;
            break;
        }
    }
}

// Update Bullets
void bullet_update(Bullet* bullets, int maxBullets, Obstacle* obstacles, int maxObstacles) {
    double deltaTime = GetFrameTime();

    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].active) {
            bullets[i].Pos = Vector3Add(bullets[i].Pos, Vector3Scale(bullets[i].Vel, deltaTime));
            
            
            for (int j = 0; j < maxObstacles; j++) {
                BoundingBox obstacleBox = GetBoundingBox(obstacles[j].position, obstacles[j].size);
                
                if (CheckCollisionBoxSphere(obstacleBox, bullets[i].Pos, bullets[i].size)) {
                    bullets[i].active = false;
                    break;
                }
            }

            bullets[i].life -= deltaTime;
            if (bullets[i].life <= 0) {
                bullets[i].active = false;
            }
        }
    }
}