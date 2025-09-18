#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

typedef struct {
    Vector3 position;
    Vector3 size;
    Vector3 direction;
    short int health;

    bool alive;
    bool enraged;

    float cooldown;
    float lastAttackTime;
} BaseEnemy;

#endif