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
        obstacles_init(obstacles);

        Init_Maurice(maurices);

        player.body.position = (Vector3){0.0f, 2.0f, 0.0f};
        player.health = 8;
        
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
        equippedWeapon = WEAPON_PLASMAGUN;
        break;
    case KEY_TWO:
        equippedWeapon = WEAPON_TEASPRAY;
        break;
    }
    //----------------------------------------------------------------------------------
    // Handle Bullet Shooting
    Vector3 camForwardDir = Vector3Normalize(Vector3Subtract(camera.target, camera.position));

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        int activePlayerBullets = 0;

        for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
            if (playerBullets[i].active) {
                activePlayerBullets++;
            }
        }

        if (GetTime() - lastShotTime >= firerate && activePlayerBullets < MAX_PLAYER_BULLETS) {

            SetSoundPitch(fxShoot, 1.0f + (GetRandomValue(-100, 100) * 0.001));
            PlaySound(fxShoot);

            bullet_spawn(playerBullets, MAX_PLAYER_BULLETS, player.body.position, camForwardDir,
                         100, 0.35f, 2.0f);
            lastShotTime = GetTime();
        }
    }

    bullet_update(playerBullets, MAX_PLAYER_BULLETS, obstacles, MAX_OBSTACLES);
    bullet_update(enemyBullets, MAX_ENEMY_BULLETS, obstacles, MAX_OBSTACLES);

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
            DrawBillboard(camera, teapotTexture, MuriceDrawPos, 3.0f,
                          maurices[i].enemy.enraged ? RED : WHITE);
        }

        if (maurices[i].exploding) {
            Rectangle src = {boomAnimTexture.width * maurices[i].animPlayer.currentFrame, 0,
                             (float)boomAnimTexture.width / boomAnim.framesCount,
                             boomAnimTexture.height};
            DrawBillboardRec(camera, boomAnimTexture, src, MuriceDrawPos, (Vector2){5.0f, 5.0f},
                             WHITE);

            maurices[i].animPlayer.frameTimer++;
            if (maurices[i].animPlayer.frameTimer >= boomAnim.frameDelay) {
                maurices[i].animPlayer.currentFrame++;

                if (maurices[i].animPlayer.currentFrame >= boomAnim.framesCount)
                    maurices[i].exploding = false;

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