#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"

typedef struct {
    Texture2D pTex;
    int x;
    int y;
    int tileX;
    int tileY;
    float acceleration;
    float maxSpeed;
    bool jumping;
    bool falling;
    bool sprinting;
    int powerup;
    bool facingRight;
    float currentSpeed;
    float speedY;
    bool onGround;
    int coins;
} playe;

extern playe player;

void initPlayer();
void dieScreen();
void winScreen();
void playerInput();
void playerPhysics();
void drawPlayer();
void playerCamera();
void callIfTouched();
int coordsToTile(int coord);

#endif