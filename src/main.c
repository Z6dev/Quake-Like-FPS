#include "emscripten/emscripten.h"
#include "raylib.h"
#include "rlgl.h"

#include <stdbool.h>

#include "entities/animation.h"
#include "entities/body.h"
#include "entities/camera_fps.h"

#include "globals/globals.h"
#include "globals/game.h"

//----------------------------------------------------------------------------------
// Obstacles
//----------------------------------------------------------------------------------

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

    // Preload Boom Gif
    boomAnim.framesCount = 0;
    boomAnim.Sprite = LoadImageAnim("resources/sprites/boom.gif", &boomAnim.framesCount);
    boomAnim.frameDelay = 6;
    boomAnimTexture = LoadTextureFromImage(boomAnim.Sprite);

    // Initialize player Variables
    player.body = (Body){0};
    player.body.position = (Vector3){0.0f, 2.0f, 0.0f};
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
    unloadSound(fxBoom);
    unloadSound(fxLaser);
    UnloadTexture(boomGifTexture); // <-- Unload texture at exit
#endif
    CloseAudioDevice(); // Close Audio Device
    CloseWindow();      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}