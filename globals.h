#ifndef GLOBALS_H
#define GLOBALS_H

#include "raylib.h"

#define SCREEN_W 320
#define SCREEN_H 240
#define WORLD_W 300
#define WORLD_H 50
#define TILE_SIZE 8
#define PLAYER_HITBOX 7
#define TOTAL_BLOCKS 13

typedef enum { MENU, EDITOR, PLATFORMER } GameState;

typedef struct block block;

struct block {
    Texture2D sprite;
    int blockID;
    bool solid;
    bool coyoteTime;
    bool isUnique;
    void (*ifTouched)(void);
    bool showInEdtior;
};

extern GameState currentState;
extern unsigned char worldMap[WORLD_H][WORLD_W];
extern Camera2D camera;
extern bool redTileOn;
extern Vector2 virtualMouse;
extern Sound coinPickup;
extern block blocks[TOTAL_BLOCKS];
extern Vector2 flagPos;
extern Vector2 endPos;
extern int currentTile;
extern bool editing;
extern Texture2D play;
extern Texture2D trash;
#endif