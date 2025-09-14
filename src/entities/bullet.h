#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

// Bullet structure
typedef struct {
    Vector3 Pos;
    Vector3 Vel;
    bool active;
    double life;
} Bullet;

void bullet_init(Bullet* bullets, int maxBullets);
void bullet_spawn(Bullet* bullets, int maxBullets, Vector3 StartPos, Vector3 Dir, float speed);
void bullet_update(Bullet* bullets, int maxBullets);

#endif