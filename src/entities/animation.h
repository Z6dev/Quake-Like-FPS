#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"

typedef struct {
    Image Sprite;
    int framesCount;
    int frameDelay;
} AnimatedSprite;

#endif