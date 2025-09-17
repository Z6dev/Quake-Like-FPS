#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>

#include "../entities/animation.h"
#include "../entities/body.h"
#include "../entities/bullet.h"
#include "../entities/camera_fps.h"
#include "../entities/enemies/maurice.h"
#include "globals.h"

void obstacles_init() {
    obstacles[0] = (Obstacle){(Vector3){3.0f, 0.0f, 0.0f}, (Vector3){3.0f, 10.0f, 3.0f}};
    obstacles[1] = (Obstacle){(Vector3){-3.0f, 0.0f, 0.0f}, (Vector3){3.0f, 10.0f, 3.0f}};

    obstacles[2] = (Obstacle){(Vector3){3.0f, 0.0f, 3.0f}, (Vector3){8.0f, 0.5f, 3.0f}};
    obstacles[3] = (Obstacle){(Vector3){3.0f, 0.5f, 3.0f}, (Vector3){5.0f, 0.5f, 3.0f}};
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Main Game Loop
void GameLoop(void) {
    // Inits
    //---------------------------------------------------
    if (!GameInitialized) {
        bullet_init(playerBullets, MAX_PLAYER_BULLETS);
        bullet_init(enemyBullets, MAX_PLAYER_BULLETS);
        obstacles_init();

        Init_Maurice(maurices);
        GameInitialized = true;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!IsCursorHidden()) {
            DisableCursor();
        }
    }

    static double lastShotTime;
    static float walkTimer;

    float dt = GetFrameTime();
    walkTimer += dt;

    //----------------------------------------------------------------------------------
    // Handle Weapons management

    switch (GetKeyPressed()) {
    case KEY_ONE:
        equippedWeapon = PLASMAGUN;
        break;
    case KEY_TWO:
        equippedWeapon = KETTLEPULT;
        break;
    }
    //----------------------------------------------------------------------------------
    // Handle Bullet Shooting

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        int activePlayerBullets = 0;
        for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
            if (playerBullets[i].active) {
                activePlayerBullets++;
            }
        }

        if (GetTime() - lastShotTime >= firerate && activePlayerBullets < MAX_PLAYER_BULLETS) {
            Vector3 camForwardDir =
                Vector3Normalize(Vector3Subtract(camera.target, camera.position));

            SetSoundPitch(fxShoot, 1.0f + (GetRandomValue(-100, 100) * 0.001));
            PlaySound(fxShoot);

            bullet_spawn(playerBullets, MAX_PLAYER_BULLETS, player.body.position, camForwardDir,
                         100, 0.35f, 2.0f);
            lastShotTime = GetTime();
        }
    }

    bullet_update(playerBullets, MAX_PLAYER_BULLETS, obstacles, MAX_OBSTACLES);

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!playerBullets[i].active)
            continue;
        for (int j = 0; j < MAX_MAURICES; j++) {
            BoundingBox MauriceBox =
                GetBoundingBox(maurices[j].enemy.position, maurices[j].enemy.size);

            if (maurices[j].enemy.alive &&
                CheckCollisionBoxSphere(MauriceBox, playerBullets[i].Pos, playerBullets[i].size)) {
                playerBullets[i].active = false;
                maurices[j].enemy.health -= 1;
                break; // Only one Maurice can be damaged per bullet
            }
        }
    }

    // Update Player
    //----------------------------------------------------------------------------------
    Vector2 mouseDelta = GetMouseDelta();
    player.lookRotation.x -= mouseDelta.x * sensitivity.x;
    player.lookRotation.y += mouseDelta.y * sensitivity.y;

    float sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    float forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    bool crouching = IsKeyDown(KEY_E);

    UpdatePlayerBody(&player.body, player.lookRotation.x, sideway, forward, IsKeyPressed(KEY_SPACE),
                     crouching, obstacles, MAX_OBSTACLES);

    // Handle Player SFX
    if (player.body.justJumped) {
        SetSoundPitch(fxJump, 1.0f + (GetRandomValue(-100, 100) * 0.001));
        PlaySound(fxJump);
    }

    if (player.body.isGrounded) {
        player.body.MoveSpeed = 20.0f;
        float walkPitch = crouching ? 0.5f : 1.0f;
        if ((forward != 0 || sideway != 0) && walkTimer > 1.5f * (crouching + 1)) {
            SetSoundPitch(fxWalk, walkPitch + (GetRandomValue(-100, 100) * 0.001));
            PlaySound(fxWalk);
            walkTimer = 0.0f;
        }
        if ((forward == 0 && sideway == 0) || IsKeyPressed(KEY_E)) {
            StopSound(fxWalk);
            walkTimer = 3.0f;
        }
    }
    if (!player.body.isGrounded) {
        player.body.MoveSpeed = 30.0f;
        StopSound(fxWalk);
        walkTimer = 3.0f;
    }

    //----------------------------------------------------------------------------------
    // Update Maurices
    for (int i = 0; i < MAX_MAURICES; i++) {
        Update_Maurice(&player, &maurices[i], enemyBullets, MAX_ENEMY_BULLETS, obstacles,
                       MAX_OBSTACLES, &fxLaser);

        if (maurices[i].justDied) {
            float distance = Vector3Distance(maurices[i].enemy.position, player.body.position);
            float volume = 15.0f / (15.0f + distance);

            SetSoundVolume(fxBoom, volume);
            SetSoundPitch(fxBoom, 1.0f + (GetRandomValue(-100, 100) * 0.001));
            PlaySound(fxBoom);

            maurices[i].justDied = false;
        }
    }

    //----------------------------------------------------------------------------------
    // Handle Camera and Camera Effects

    // Camera Height Handler, checks when crouching or standing.
    player.headLerp = Lerp(player.headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.0f * dt);
    camera.position = (Vector3){
        player.body.position.x,
        player.body.position.y + (BOTTOM_HEIGHT + player.headLerp),
        player.body.position.z,
    };

    if (player.body.isGrounded && ((forward != 0) || (sideway != 0))) {
        player.headTimer += dt * 3.0f;
        player.walkLerp = Lerp(player.walkLerp, 1.0f, 10.0f * dt);
        camera.fovy = Lerp(camera.fovy, 70.0f, 5.0f * dt);
    } else {
        player.walkLerp = Lerp(player.walkLerp, 0.0f, 10.0f * dt);
        camera.fovy = Lerp(camera.fovy, 60.0f, 5.0f * dt);
    }

    player.lean.x = Lerp(player.lean.x, sideway * 0.04f, 10.0f * dt);
    player.lean.y = Lerp(player.lean.y, forward * 0.03f, 10.0f * dt);

    UpdateCameraFPS(&camera, &player);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground((Color){30, 30, 30, 255});

    BeginMode3D(camera);

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!playerBullets[i].active == true) {
            continue;
        }

        DrawSphere(playerBullets[i].Pos, playerBullets[i].size, BLUE);
        DrawSphereWires(playerBullets[i].Pos, playerBullets[i].size + 0.1f, 6, 6, DARKBLUE);
    }

    DrawLevel();
    EndMode3D();

    // Draw Debug Info
    DrawFPS(screenWidth - 100, 0);

    // Equipped weapons debug info
    char WeaponBuf[18];
    snprintf(WeaponBuf, 18, "Equipped: %d", equippedWeapon);
    DrawText(WeaponBuf, 0, 0, 23, DARKGREEN);

    // Draw Crosshair
    DrawCircleLines(screenWidth / 2, (screenHeight / 2) + 5, 5, DARKGRAY);

    EndDrawing();
    //----------------------------------------------------------------------------------
}

// Draw game level
void DrawLevel(void) {
    const int floorExtent = 25;
    const float tileSize = 5.0f;
    const Color tileColor = {168, 0, 219, 255};

    // Floor tiles
    for (int y = -floorExtent; y < floorExtent; y++) {
        for (int x = -floorExtent; x < floorExtent; x++) {
            if ((y & 1) && (x & 1)) {
                DrawPlane((Vector3){x * tileSize, 0.0f, y * tileSize},
                          (Vector2){tileSize, tileSize}, tileColor);
            } else if (!(y & 1) && !(x & 1)) {
                DrawPlane((Vector3){x * tileSize, 0.0f, y * tileSize},
                          (Vector2){tileSize, tileSize}, tileColor);
            }
        }
    }

    // Draw Obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        Vector3 ObstacleDrawPos =
            (Vector3){obstacles[i].position.x, obstacles[i].position.y + obstacles[i].size.y / 2.0f,
                      obstacles[i].position.z};

        DrawBoundingBox(GetBoundingBox(obstacles[i].position, obstacles[i].size), RED);
        DrawCubeV(ObstacleDrawPos, obstacles[i].size, PURPLE);
        DrawCubeWiresV(ObstacleDrawPos, obstacles[i].size, DARKPURPLE);
    }

    DrawMaurices();

    // Draw Player
    DrawBoundingBox(GetBoundingBox(player.body.position, (Vector3){1.0f, 2.0f, 1.0f}), GREEN);

    // Red sun
    DrawSphere((Vector3){300.0f, 300.0f, 0.0f}, 100.0f, (Color){255, 0, 0, 255});
}

void DrawMaurices(void) {

    // Draw Maurices
    for (int i = 0; i < MAX_MAURICES; i++) {
        Vector3 MuriceDrawPos =
            (Vector3){maurices[i].enemy.position.x,
                      maurices[i].enemy.position.y + maurices[i].enemy.size.y / 2.0f,
                      maurices[i].enemy.position.z};

        if (maurices[i].enemy.alive) {
            Vector3 dir = maurices[i].enemy.direction;
            float yaw = atan2f(dir.x, dir.z) * RAD2DEG;
            float pitch = -atan2f(dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z)) * RAD2DEG;

            rlPushMatrix();
            rlTranslatef(MuriceDrawPos.x, MuriceDrawPos.y, MuriceDrawPos.z);

            rlRotatef(yaw, 0.0f, 1.0f, 0.0f);
            rlRotatef(pitch, 1.0f, 0.0f, 0.0f);

            DrawCubeV(Vector3Zero(), maurices[i].enemy.size,
                      maurices[i].enemy.enraged ? RED : GRAY);
            DrawCubeWiresV(Vector3Zero(), maurices[i].enemy.size, RAYWHITE);
            rlPopMatrix();
        }

        if (maurices[i].exploding) {
            DrawBillboard(camera, boomAnimTexture, MuriceDrawPos, 7.0f, WHITE);

            maurices[i].animPlayer.frameTimer++;
            if (maurices[i].animPlayer.frameTimer >= boomAnim.frameDelay) {
                maurices[i].animPlayer.currentFrame++;

                if (maurices[i].animPlayer.currentFrame >= boomAnim.framesCount)
                    maurices[i].exploding = false;

                maurices[i].animPlayer.nextFrameDataOffset = boomAnim.Sprite.width *
                                                             boomAnim.Sprite.height * 4 *
                                                             maurices[i].animPlayer.currentFrame;

                UpdateTexture(boomAnimTexture, ((unsigned int*)boomAnim.Sprite.data) +
                                                   maurices[i].animPlayer.nextFrameDataOffset);

                maurices[i].animPlayer.frameTimer = 0;
            }
        }
    }

    // Draw Maurice Bullets

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemyBullets[i].active) {
            DrawSphere(enemyBullets[i].Pos, enemyBullets[i].size, ORANGE);
        }
    }
}