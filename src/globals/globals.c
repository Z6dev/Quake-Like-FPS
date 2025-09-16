#include "globals.h"

// Weapons

enum Weapon equippedWeapon;

int screenWidth = 800;
int screenHeight = 450;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

bool GameInitialized = false;

float firerate = 0.1f;

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