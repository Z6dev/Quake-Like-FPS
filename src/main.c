#include "emscripten/emscripten.h"
#include "raylib.h"
#include "raymath.h"

#include <stdbool.h>

#include "entities/body.h"
#include "entities/bullet.h"
#include "entities/camera_fps.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define CROUCH_HEIGHT 0.0f
#define STAND_HEIGHT 1.0f
#define BOTTOM_HEIGHT 0.5f

//----------------------------------------------------------------------------------
// Obstacles
//----------------------------------------------------------------------------------

#define MAX_OBSTACLES 5
static Obstacle obstacles[MAX_OBSTACLES];

void obstacles_init() {
    obstacles[0] = (Obstacle){(Vector3){3.0f, 0.0f, 0.0f}, (Vector3){3.0f, 10.0f, 3.0f}};
    obstacles[1] = (Obstacle){(Vector3){-3.0f, 0.0f, 0.0f}, (Vector3){3.0f, 10.0f, 3.0f}};

    obstacles[2] = (Obstacle){(Vector3){3.0f, 0.0f, 3.0f}, (Vector3){8.0f, 0.5f, 3.0f}};
}

//----------------------------------------------------------------------------------
// Bullets
//----------------------------------------------------------------------------------

#define MAX_PLAYER_BULLETS 64
static Bullet playerBullets[MAX_PLAYER_BULLETS];

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 450;
bool GameInitialized = false;

float firerate = 0.1f;

Vector2 sensitivity = {0.005f, 0.005f};

Player player = {0};
Camera3D camera = {0};

Sound fxShoot;
Sound fxWalk;
Sound fxJump;
Music bgOst;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void GameLoop(void);
void DrawLevel(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "Exercitus Mauris");
    InitAudioDevice();
    SetMasterVolume(1.5f);

    bullet_init(playerBullets, MAX_PLAYER_BULLETS);
    obstacles_init();

    // Preload Meshes
    //---------------------------------------------------
    bgOst = LoadMusicStream("resources/bg.mp3");
    fxShoot = LoadSound("resources/chaingun.ogg");
    fxWalk = LoadSound("resources/walk.mp3");
    fxJump = LoadSound("resources/ha.mp3");
    SetMusicVolume(bgOst, 0.0f);

    // Initialize player Variables
    player.body = (Body){0};
    player.lookRotation = (Vector2){0};
    player.headTimer = 0.0f;
    player.walkLerp = 0.0f;
    player.headLerp = STAND_HEIGHT;
    player.lean = (Vector2){0};

    // Initialize camera variables
    // NOTE: UpdateCameraFPS() takes care of the rest
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = (Vector3){
        player.body.position.x,
        player.body.position.y + (BOTTOM_HEIGHT + player.headLerp),
        player.body.position.z,
    };

    UpdateCameraFPS(&camera, &player); // Update camera parameters

    //--------------------------------------------------------------------------------------

#ifdef __EMSCRIPTEN_major__
    emscripten_set_main_loop(GameLoop, 60, 1);
#else
    while (!WindowShouldClose()) {
        GameLoop();
    }
#endif

// De-Initialization
//--------------------------------------------------------------------------------------
#ifndef __EMSCRIPTEN_major__
    UnloadSound(fxJump);
    UnloadSound(fxShoot);
    UnloadSound(fxWalk);
#endif
    CloseAudioDevice(); // Close Audio Device
    CloseWindow();      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Main Game Loop
void GameLoop(void) {
    // Inits
    //---------------------------------------------------
    if (!GameInitialized) {
        PlayMusicStream(bgOst);
        GameInitialized = true;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!IsCursorHidden()) {
            DisableCursor();
        }
    }

    UpdateMusicStream(bgOst);
    

    static double lastShotTime;
    static float walkTimer;

    float dt = GetFrameTime();
    walkTimer += dt;

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
                         100);
            lastShotTime = GetTime();
        }
    }

    bullet_update(playerBullets, MAX_PLAYER_BULLETS);

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

    // Collisions testing
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground((Color){30, 30, 30, 255});

    BeginMode3D(camera);

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!playerBullets[i].active == true) {
            continue;
        }

        DrawSphere(playerBullets[i].Pos, 0.7, BLUE);
        DrawSphereWires(playerBullets[i].Pos, 0.8, 6, 6, DARKBLUE);
    }

    DrawLevel();
    EndMode3D();

    // Draw Debug Info
    DrawFPS(screenWidth - 100, 0);

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

    DrawBoundingBox(GetBoundingBox(player.body.position, (Vector3){1.0f, 2.0f, 1.0f}), GREEN);

    // Red sun
    DrawSphere((Vector3){300.0f, 300.0f, 0.0f}, 100.0f, (Color){255, 0, 0, 255});
}