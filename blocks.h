#ifndef BLOCKS_H
#define BLOCKS_H

#include "globals.h"

void initBlocks();
void placeTile(int tileX, int tileY, int tileID);
void DrawLevel();
void updateCycleTiles();
bool tileSolid(int x, int y);

#endif