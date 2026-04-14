#ifndef ENEMY_H
#define ENEMY_H

#include "globals.h"
#define HEDGEHOG_SPEED 0.5f
typedef struct {
    float x;
    float y;
    bool left;
    bool active;
} enemyHedgehog;
float important_hedgehog_meaning_of_life( float number );
extern enemyHedgehog* hedgehogs;
extern int hedgehogCount;
extern int currentMaxHedgehog;

void addHedgehog(float x, float y);
void updateHedgehog();
void drawHedgehog();
void clearHedgehog();

typedef struct {
    float x;
    float y;
    bool left;
} movingPad;

extern movingPad* pads;
extern int padCount;
extern int currentMaxPad;

void addPad(float x, float y);
void updatePad();
void drawPad();
void clearPad();
bool padSolid(float x, float y);
#endif