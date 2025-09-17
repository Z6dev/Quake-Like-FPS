#include "maurice.h"
#include "../body.h"
#include "../bullet.h"
#include <raylib.h>
#include <raymath.h>

// Actual Functions
void Update_Maurice(Player* player, Maurice* murice, Bullet* enemyBullets, int bulletArr_size,
                    Obstacle* obstacles, int obstacleArr_size, Sound* fxShoot) {

    bullet_update(enemyBullets, bulletArr_size, obstacles, obstacleArr_size);

    if (!murice->enemy.alive)
        return;

    //---------------------------------------------------
    // Physics Stuff

    // Get vector from Maurice to player
    Vector3 direction = Vector3Subtract(
        (Vector3){player->body.position.x, player->body.position.y, player->body.position.z},
        murice->enemy.position);
    float distance = Vector3Length(direction);

    // Normalize direction
    if (distance > 0.0f) {
        murice->enemy.direction = Vector3Scale(direction, 1.0f / distance);
    }

    float stopDistance = 4.0f; // how close he should get before stopping
    float speed = 0.05f;       // how fast Maurice moves

    if (distance > stopDistance) {
        // Move Maurice toward player
        Vector3 moveTowardsDir =
            Vector3Subtract((Vector3){player->body.position.x, player->body.position.y + 2.5f,
                                      player->body.position.z},
                            murice->enemy.position);

        murice->enemy.position = Vector3Add(murice->enemy.position,
                                            Vector3Scale(Vector3Normalize(moveTowardsDir), speed));
    }

    //------------------------------------------------------------------
    // Check if maurice should be dead by Now
    if (murice->enemy.health <= 0 && murice->enemy.alive) {
        murice->justDied = true;
        murice->enemy.alive = false;
        murice->exploding = true;
        murice->animPlayer.currentFrame = 0;
        murice->animPlayer.frameTimer = 0;
    }

    if (murice->enemy.health < MAURICE_MAX_HEALTH / 2 && !murice->enemy.enraged) {
        murice->enemy.enraged = true;
        murice->enemy.cooldown =
            murice->enemy.enraged ? MAURICE_ENRAGED_COOLDOWN : MAURICE_NORMAL_COOLDOWN;
        murice->firerate =
            murice->enemy.enraged ? MAURICE_ENRAGED_FIRERATE : MAURICE_NORMAL_FIRERATE;
    }

    //-------------------------------------------------------------------
    // Maurice Attack
    int maxShot = murice->enemy.enraged ? MAURICE_ENRAGED_MAXSHOT : MAURICE_NORMAL_MAXSHOT;

    if ((float)GetTime() - murice->enemy.lastAttackTime >= murice->enemy.cooldown) {
        if ((float)GetTime() - murice->lastShotTime >= murice->firerate &&
            murice->shotCount <= maxShot) {
            bullet_spawn(enemyBullets, bulletArr_size, murice->enemy.position,
                         murice->enemy.direction, murice->enemy.enraged ? 25.0f : 15.0f, 0.5f,
                         10.0f);

            float distance = Vector3Distance(murice->enemy.position, player->body.position);
            float volume = 15.0f / (15.0f + distance);

            SetSoundVolume(*fxShoot, volume);
            SetSoundPitch(*fxShoot, 1.0f + (GetRandomValue(-100, 100) * 0.001));
            PlaySound(*fxShoot);

            murice->lastShotTime = (float)GetTime();
            murice->shotCount++;
        }
    }

    if (murice->shotCount >= maxShot) {
        murice->enemy.lastAttackTime = (float)GetTime() + (GetRandomValue(-6, 15) * 0.2);
        murice->shotCount = 0;
    }
}

void Init_Maurice(Maurice* maurices) {
    for (int i = 0; i < MAX_MAURICES; i++) {
        maurices[i].enemy = (BaseEnemy){
            (Vector3){GetRandomValue(-25, 10), GetRandomValue(3, 15), GetRandomValue(-25, 10)},
            (Vector3){2.0f, 3.0f, 2.0f}};
        maurices[i].enemy.direction = (Vector3){0, 0, 0};
        maurices[i].enemy.alive = true;
        maurices[i].enemy.health = 30;

        maurices[i].exploding = false;
        maurices[i].animPlayer.frameTimer = 0;
        maurices[i].animPlayer.currentFrame = 0;

        maurices[i].firerate = MAURICE_NORMAL_FIRERATE;
        maurices[i].enemy.cooldown = MAURICE_NORMAL_COOLDOWN;
    }
}
