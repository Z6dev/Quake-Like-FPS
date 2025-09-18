#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include "raylib.h"

//------------------------------------------------------------------------------------
// Custom Functions Declaration
//------------------------------------------------------------------------------------
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length,
                     Color color); // Draw cube textured
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width,
                        float height, float length,
                        Color color); // Draw cube with a region of a texture

#endif