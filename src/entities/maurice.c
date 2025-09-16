#include "maurice.h"
#include "body.h"
#include "bullet.h"
#include <raylib.h>
#include <raymath.h>

// Actual Functions
void Update_Maurice(Player* player, Maurice* murice, Bullet* enemyBullets, int bulletArr_size,
                    Obstacle* obstacles, int obstacleArr_size, Sound* fxShoot) {
    if (!murice->alive)
        return;

    //---------------------------------------------------
    // Physics Stuff

    // Get vector from Maurice to player
    Vector3 direction = Vector3Subtract(
        (Vector3){player->body.position.x, player->body.position.y, player->body.position.z},
        murice->obstacle.position);
    float distance = Vector3Length(direction);

    // Normalize direction
    if (distance > 0.0f) {
        murice->direction = Vector3Scale(direction, 1.0f / distance);
    }

    float stopDistance = 4.0f; // how close he should get before stopping
    float speed = 0.05f;       // how fast Maurice moves

    if (distance > stopDistance) {
        // Move Maurice toward player
        Vector3 moveTowardsDir =
            Vector3Subtract((Vector3){player->body.position.x, player->body.position.y + 2.5f,
                                      player->body.position.z},
                            murice->obstacle.position);

        murice->obstacle.position = Vector3Add(
            murice->obstacle.position, Vector3Scale(Vector3Normalize(moveTowardsDir), speed));
    }

    //------------------------------------------------------------------
    // Check if maurice should be dead by Now
    if (murice->health <= 0 && murice->alive) {
        murice->justDied = true;
        murice->alive = false;
        murice->exploding = true;
        murice->currentFrame = 0;
        murice->frameTimer = 0;
    }

    if (murice->health < MAURICE_MAX_HEALTH / 2 && !murice->enraged) {
        murice->enraged = true;
        murice->cooldown = murice->enraged ? MAURICE_ENRAGED_COOLDOWN : MAURICE_NORMAL_COOLDOWN;
        murice->firerate = murice->enraged ? MAURICE_ENRAGED_FIRERATE : MAURICE_NORMAL_FIRERATE;
    }

    //-------------------------------------------------------------------
    // Maurice Attack
    int maxShot = murice->enraged ? MAURICE_ENRAGED_MAXSHOT : MAURICE_NORMAL_MAXSHOT;

    if ((float)GetTime() - murice->lastAttackTime >= murice->cooldown) {
        if ((float)GetTime() - murice->lastShotTime >= murice->firerate &&
            murice->shotCount <= maxShot) {
            bullet_spawn(enemyBullets, bulletArr_size, murice->obstacle.position, murice->direction,
                         murice->enraged ? 25.0f : 15.0f, 0.5f, 10.0f);

            float distance = Vector3Distance(murice->obstacle.position, player->body.position);
            float volume = 15.0f / (15.0f + distance);

            SetSoundVolume(*fxShoot, volume);
            PlaySound(*fxShoot);
            murice->lastShotTime = (float)GetTime();
            murice->shotCount++;
        }
    }

    if (murice->shotCount >= maxShot) {
        murice->lastAttackTime = (float)GetTime() + (GetRandomValue(-6, 15) * 0.2);
        murice->shotCount = 0;
    }

    bullet_update(enemyBullets, bulletArr_size, obstacles, obstacleArr_size);
}

void Init_Maurice(Maurice* maurices) {
    for (int i = 0; i < MAX_MAURICES; i++) {
        maurices[i].obstacle = (Obstacle){
            (Vector3){GetRandomValue(-25, 10), GetRandomValue(3, 15), GetRandomValue(-25, 10)},
            (Vector3){2.0f, 3.0f, 2.0f}};
        maurices[i].direction = (Vector3){0, 0, 0};
        maurices[i].alive = true;
        maurices[i].health = 30;

        maurices[i].exploding = false;
        maurices[i].frameTimer = 0;
        maurices[i].currentFrame = 0;

        maurices[i].firerate = MAURICE_NORMAL_FIRERATE;
        maurices[i].cooldown = MAURICE_NORMAL_COOLDOWN;
    }
}
