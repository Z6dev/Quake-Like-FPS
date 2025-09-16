#ifndef GLOBALS_H
#define GLOBALS_H

#include "raylib.h"

#include "../entities/body.h"
#include "../entities/bullet.h"
#include "../entities/animation.h"
#include "../entities/maurice.h"

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
// Weapons
//----------------------------------------------------------------------------------

enum Weapon { PLASMAGUN, BBGUN };
extern enum Weapon equippedWeapon;

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

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void GameLoop(void);
void DrawLevel(void);
void DrawMaurices(void);

#endif