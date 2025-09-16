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

#include "body.h"
#include "bullet.h"

typedef struct {
    Obstacle obstacle;
    Vector3 direction;
    short int health;

    bool alive;
    bool justDied;
    bool exploding;
    bool enraged;

    float lastShotTime;
    float firerate;
    float cooldown;
    float lastAttackTime;

    int shotCount;

    int currentFrame;
    int frameTimer;
    unsigned int nextFrameDataOffset;
} Maurice;

void Update_Maurice(Player* player, Maurice* murice, Bullet* enemybullet_arr, int bulletArr_size,
                    Obstacle* obstacles, int obstacleArr_size, Sound* fxShoot);
void Init_Maurice(Maurice* maurice);

#endif