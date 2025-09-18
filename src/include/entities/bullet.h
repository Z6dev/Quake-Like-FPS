#ifndef BULLET_H
#define BULLET_H

#include "body.h"
#include "raylib.h"

// Bullet structure
typedef struct {
    Vector3 position;
    Vector3 Vel;
    float size;
    bool active;
    double life;
} Bullet;

void bullet_init(Bullet* bullets, int maxBullets);
void bullet_spawn(Bullet* bullets, int maxBullets, Vector3 StartPos, Vector3 Dir, float speed,
                  float size, float life);
void bullet_update(Bullet* bullets, int maxBullets, Obstacle* obstacles, int maxObstacles);

#endif