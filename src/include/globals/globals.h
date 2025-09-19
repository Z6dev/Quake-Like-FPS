#ifndef GLOBALS_H
#define GLOBALS_H

#include "raylib.h"

#include "../entities/animation.h"
#include "../entities/body.h"
#include "../entities/bullet.h"
#include "../entities/enemies/maurice.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

#define CROUCH_HEIGHT 0.0f
#define STAND_HEIGHT 1.0f
#define BOTTOM_HEIGHT 0.5f

#define FAR_CULL_DISTANCE 100.0f

#define MAX_OBSTACLES 5
extern Obstacle obstacles[];

//----------------------------------------------------------------------------------
// Bullets
//----------------------------------------------------------------------------------

#define MAX_PLAYER_BULLETS 64
static Bullet playerBullets[MAX_PLAYER_BULLETS];

#define MAX_ENEMY_BULLETS 100
static Bullet enemyBullets[MAX_ENEMY_BULLETS];

//----------------------------------------------------------------------------------
// Maurice Enemies
//----------------------------------------------------------------------------------

static Maurice maurices[MAX_MAURICES];

//----------------------------------------------------------------------------------
// Enums
//----------------------------------------------------------------------------------

enum Weapon { WEAPON_PLASMAGUN, WEAPON_TEASPRAY };
extern enum Weapon equippedWeapon;

enum PlayerState { SCENE_GAME, SCENE_MENU, SCENE_PAUSE };
extern enum PlayerState GameScene;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern int screenWidth;
extern int screenHeight;

extern bool GameInitialized;

extern float firerate;

extern Vector2 sensitivity;

extern Player player;
extern Camera3D camera;

extern Sound fxShoot;
extern Sound fxWalk;
extern Sound fxJump;
extern Sound fxBoom;
extern Sound fxLaser;

extern AnimatedSprite boomAnim;
extern Texture2D boomAnimTexture;

extern Texture2D teapotTexture;
extern Texture2D studTexture;

// ====================================== SHADERS ==========================================

#endif