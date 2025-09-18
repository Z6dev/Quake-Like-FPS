#include "emscripten/emscripten.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "entities/animation.h"
#include "entities/body.h"
#include "entities/camera_fps.h"

#include "globals/game.h"
#include "globals/globals.h"

#include <math.h>

//----------------------------------------------------------------------------------
// Global Constants Definition
//----------------------------------------------------------------------------------

enum Weapon equippedWeapon = WEAPON_PLASMAGUN;
enum PlayerState gameState = SCENE_MENU;

int screenWidth = 800;
int screenHeight = 450;

Obstacle obstacles[MAX_OBSTACLES];

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

bool GameInitialized = false;

float firerate = 0.15f;

Vector2 sensitivity = {0.005f, 0.005f};

Player player = {0};
Camera3D camera = {0};

Sound fxShoot;
Sound fxWalk;
Sound fxJump;
Sound fxBoom;

Sound fxLaser;

AnimatedSprite boomAnim = {0};
Texture2D boomAnimTexture;

Texture2D teapotTexture;
Texture2D studTexture;

//----------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------

void MainLoop() {
    switch (gameState) {
    case SCENE_GAME:
        GameLoop();
        break;

    case SCENE_MENU:
        MenuLoop();
        break;
    }
}

void MenuLoop() {
    BeginDrawing();

    for (int y = -screenHeight; y < screenHeight * 2; y += studTexture.height / 4) {
        for (int x = 0; x < screenWidth; x += studTexture.width / 4) {
            DrawTexturePro(studTexture, (Rectangle){0, 0, studTexture.width, studTexture.height},
                           (Rectangle){x, sinf((float)GetTime()) * 100 + y,
                                       studTexture.width / 4.0f, studTexture.height / 4.0f},
                           (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        }
    }

    int textWidth = MeasureText("EXERCITUS MAURIS", 48);
    DrawText("EXERCITUS MAURIS", screenWidth / 2 - textWidth / 2, 100, 48, WHITE);

    if (GuiButton((Rectangle){screenWidth/2.0f - 110.0f, 200.0f, 220.0f, 80.0f}, "New Game")) {
        GameInitialized = false;
        gameState = SCENE_GAME;
    }

    EndDrawing();
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "Exercitus Mauris");
    InitAudioDevice();
    SetMasterVolume(1.5f);


    // Preload
    //---------------------------------------------------
    fxShoot = LoadSound("resources/sfx/chaingun.ogg");
    fxWalk = LoadSound("resources/sfx/walk.mp3");
    fxJump = LoadSound("resources/sfx/ha.mp3");
    fxBoom = LoadSound("resources/sfx/boom.mp3");
    fxLaser = LoadSound("resources/sfx/lasershoot.mp3");

    teapotTexture = LoadTexture("resources/sprites/teapot.png");
    studTexture = LoadTexture("resources/sprites/stud.png");

    // Preload Boom Gif
    boomAnim.framesCount = 5;
    boomAnim.Sprite = LoadImage("resources/sprites/boom.png");
    boomAnim.frameDelay = 12;
    boomAnimTexture = LoadTextureFromImage(boomAnim.Sprite);

    // Initialize player Variables
    player.body = (Body){0};
    player.body.position = (Vector3){0.0f, 2.0f, 0.0f};
    player.lookRotation = (Vector2){0};
    player.headTimer = 0.0f;
    player.walkLerp = 0.0f;
    player.headLerp = STAND_HEIGHT;
    player.lean = (Vector2){0};
    player.health = 8;

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
    emscripten_set_main_loop(MainLoop, 60, 1);
#else
    while (!WindowShouldClose()) {
        MainLoop();
    }
#endif

// De-Initialization
//--------------------------------------------------------------------------------------
#ifndef __EMSCRIPTEN_major__
    UnloadSound(fxJump);
    UnloadSound(fxShoot);
    UnloadSound(fxWalk);
    unloadSound(fxBoom);
    unloadSound(fxLaser);

    UnloadTexture(boomGifTexture);
    UnloadTexture();
#endif
    CloseAudioDevice(); // Close Audio Device
    CloseWindow();      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}