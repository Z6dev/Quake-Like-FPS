#include "globals.h"
#include "raylib.h"

// Weapons

enum Weapon equippedWeapon;

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

AnimatedSprite boomAnim;
Texture2D boomAnimTexture;

Texture2D teapotTexture;