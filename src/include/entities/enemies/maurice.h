#ifndef MAURICE_H
#define MAURICE_H

// Constants
#define MAURICE_NORMAL_COOLDOWN 3.0f
#define MAURICE_NORMAL_FIRERATE 0.4f

#define MAURICE_ENRAGED_COOLDOWN 2.0f
#define MAURICE_ENRAGED_FIRERATE 0.2f

#define MAURICE_NORMAL_MAXSHOT 6
#define MAURICE_ENRAGED_MAXSHOT 12

#define MAX_MAURICES 5
#define MAURICE_MAX_HEALTH 30

#include "../animation.h"
#include "../body.h"
#include "../bullet.h"
#include "enemy.h"

typedef struct {
    BaseEnemy enemy;

    bool justDied;
    bool exploding;

    float lastShotTime;
    float firerate;

    int shotCount;

    AnimPlayer animPlayer;
} Maurice;

void Update_Maurice(Player* player, Maurice* murice, Bullet* enemybullet_arr, int bulletArr_size,
                    Sound* fxShoot);
void Init_Maurice(Maurice* maurice);

#endif