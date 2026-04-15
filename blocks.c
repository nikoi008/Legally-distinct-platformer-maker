#include "blocks.h"
#include "player.h"
#include <math.h>
#include <stdlib.h>
#include "globals.h"
int keys = 0;
unsigned char worldMap[WORLD_H][WORLD_W] = { 0 };
block blocks[TOTAL_BLOCKS] = { 0 };
Vector2 flagPos = { -1, -1 };
Vector2 endPos = { -1, -1 };
int currentTile = 1;
bool redTileOn = false;

void collectCoin() {
    PlaySound(coinPickup);
    worldMap[player.tileY][player.tileX] = 0;
    player.coins++;
}
void collectKey() {
    worldMap[player.tileY][player.tileX] = 0; 
    keys++;
}

void initBlocks() {
    Texture2D air = LoadTexture("sprites/blocks/air.png");
    blocks[0].sprite = air;
    blocks[0].blockID = 0;
    blocks[0].solid = false;
    blocks[0].isUnique = false;
    blocks[0].ifTouched = NULL;
    blocks[0].showInEdtior = true;

    Texture2D ground = LoadTexture("sprites/blocks/ground.png");
    blocks[1].sprite = ground;
    blocks[1].blockID = 1;
    blocks[1].solid = true;
    blocks[1].coyoteTime = true;
    blocks[1].isUnique = false;
    blocks[1].ifTouched = NULL;
    blocks[1].showInEdtior = true;

    Texture2D brick = LoadTexture("sprites/blocks/brick.png");
    blocks[2].sprite = brick;
    blocks[2].blockID = 2;
    blocks[2].solid = true;
    blocks[2].coyoteTime = true;
    blocks[2].isUnique = false;
    blocks[2].ifTouched = NULL;
    blocks[2].showInEdtior = true;

    Texture2D flag = LoadTexture("sprites/blocks/flag.png");
    blocks[3].sprite = flag;
    blocks[3].blockID = 3;
    blocks[3].solid = false;
    blocks[3].coyoteTime = false;
    blocks[3].isUnique = true;
    blocks[3].ifTouched = NULL;
    blocks[3].showInEdtior = true;

    Texture2D end = LoadTexture("sprites/blocks/end.png");
    blocks[4].sprite = end;
    blocks[4].blockID = 4;
    blocks[4].solid = false;
    blocks[4].coyoteTime = false;
    blocks[4].isUnique = true;
    blocks[4].ifTouched = winScreen;
    blocks[4].showInEdtior = true;

    Texture2D spike = LoadTexture("sprites/blocks/spike.png");
    blocks[5].sprite = spike;
    blocks[5].blockID = 5;
    blocks[5].solid = false;
    blocks[5].coyoteTime = false;
    blocks[5].isUnique = false;
    blocks[5].ifTouched = dieScreen;
    blocks[5].showInEdtior = true;

    Texture2D cycleRedOn = LoadTexture("sprites/blocks/cbRed0.png");
    blocks[6].sprite = cycleRedOn;
    blocks[6].blockID = 6;
    blocks[6].solid = true;
    blocks[6].coyoteTime = true;
    blocks[6].isUnique = false;
    blocks[6].ifTouched = NULL;
    blocks[6].showInEdtior = true;

    Texture2D cycleRedOff = LoadTexture("sprites/blocks/cbRed1.png");
    blocks[7].sprite = cycleRedOff;
    blocks[7].blockID = 7;
    blocks[7].solid = false;
    blocks[7].coyoteTime = false;
    blocks[7].isUnique = false;
    blocks[7].ifTouched = NULL;
    blocks[7].showInEdtior = false;

    Texture2D cycleBlueOn = LoadTexture("sprites/blocks/cbBlue0.png");
    blocks[8].sprite = cycleBlueOn;
    blocks[8].blockID = 8;
    blocks[8].solid = true;
    blocks[8].coyoteTime = true;
    blocks[8].isUnique = false;
    blocks[8].ifTouched = NULL;
    blocks[8].showInEdtior = true;

    Texture2D cycleBlueOff = LoadTexture("sprites/blocks/cbBlue1.png");
    blocks[9].sprite = cycleBlueOff;
    blocks[9].blockID = 9;
    blocks[9].solid = false;
    blocks[9].coyoteTime = false;
    blocks[9].isUnique = false;
    blocks[9].ifTouched = NULL;
    blocks[9].showInEdtior = false;

    Texture2D coin = LoadTexture("sprites/blocks/coin.png");
    blocks[10].sprite = coin;
    blocks[10].blockID = 10;
    blocks[10].solid = false;
    blocks[10].coyoteTime = false;
    blocks[10].isUnique = false;
    blocks[10].ifTouched = collectCoin;
    blocks[10].showInEdtior = true;

    Texture2D enemy = LoadTexture("sprites/blocks/hedge.png");
    blocks[11].sprite = enemy;
    blocks[11].blockID = 11;
    blocks[11].solid = false;
    blocks[11].coyoteTime = false;
    blocks[11].isUnique = false;
    blocks[11].ifTouched = dieScreen;
    blocks[11].showInEdtior = true;
    Texture2D movingBlock = LoadTexture("sprites/blocks/movement.png");
    blocks[12].sprite = movingBlock;
    blocks[12].blockID = 12;
    blocks[12].solid = true;
    blocks[12].coyoteTime = true;
    blocks[12].isUnique = false;
    blocks[12].ifTouched = NULL;
    blocks[12].showInEdtior = true;

    Texture2D grass = LoadTexture("sprites/blocks/grass.png");
    blocks[13].sprite = grass;
    blocks[13].blockID = 13;
    blocks[13].solid = false;
    blocks[13].coyoteTime = false;
    blocks[13].isUnique = false;
    blocks[13].ifTouched = NULL;
    blocks[13].showInEdtior = true;

    Texture2D cloud = LoadTexture("sprites/blocks/cloud.png");
    blocks[14].sprite = cloud;
    blocks[14].blockID = 14;
    blocks[14].solid = false;
    blocks[14].coyoteTime = false;
    blocks[14].isUnique = true;
    blocks[14].ifTouched = NULL;
    blocks[14].showInEdtior = true;

    Texture2D flowers = LoadTexture("sprites/blocks/flowers.png");
    blocks[15].sprite = flowers;
    blocks[15].blockID = 15;
    blocks[15].solid = false;
    blocks[15].coyoteTime = false;
    blocks[15].isUnique = false;
    blocks[15].ifTouched = NULL;
    blocks[15].showInEdtior = true;

    Texture2D key = LoadTexture("sprites/blocks/key.png");
    blocks[16].sprite = key;
    blocks[16].blockID = 16;
    blocks[16].solid = false;
    blocks[16].ifTouched = collectKey; 
    blocks[16].showInEdtior = true;

    Texture2D door = LoadTexture("sprites/blocks/door.png");
    blocks[17].sprite = door;
    blocks[17].blockID = 17;
    blocks[17].solid = true; 
    blocks[17].ifTouched = NULL; 
    blocks[17].showInEdtior = true;

    
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        SetTextureFilter(blocks[i].sprite, TEXTURE_FILTER_POINT);
    }
}

void placeTile(int tileX, int tileY, int tileID) {
    if (blocks[tileID].isUnique == true) {
        switch (tileID) {
        case 3:
            if (flagPos.x != -1) {
                worldMap[(int)flagPos.y][(int)flagPos.x] = 0;
            }
            flagPos.x = tileX;
            flagPos.y = tileY;
            break;
        case 4:
            if (endPos.x != -1) {
                worldMap[(int)endPos.y][(int)endPos.x] = 0;
            }
            endPos.x = tileX;
            endPos.y = tileY;
            break;
	case 14:
	    if (tileX + 1 < WORLD_H) {
	    worldMap[tileY][tileX + 1] = 14;
	    }
        }
    }
    if (tileID == 0 && tileX == flagPos.x && tileY == flagPos.y) {
        flagPos = (Vector2){ -1,-1 };
    }
    if (tileID == 0 && tileX == endPos.x && tileY == endPos.y) {
        flagPos = (Vector2){ -1,-1 };
    }
    worldMap[tileY][tileX] = tileID;
}
/*
void DrawLevel() {
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            int tileID = worldMap[y][x];
            if (tileID > 0 && tileID < TOTAL_BLOCKS) {
                Texture2D blockTexture = blocks[tileID].sprite;
                if (tileID == 6 || tileID == 7)
                    blockTexture = redTileOn ? blocks[6].sprite : blocks[7].sprite;
                if (tileID == 8 || tileID == 9)
                    blockTexture = redTileOn ? blocks[9].sprite : blocks[8].sprite;
                DrawTexture(blockTexture, x * TILE_SIZE, y * TILE_SIZE, WHITE);
            }
        }
    }
}*/

void DrawLevel() {
    int closestX = (int)((camera.target.x - (SCREEN_W / 2.0f)) / TILE_SIZE) - 1;
    int furthestX = (int)((camera.target.x + (SCREEN_W / 2.0f)) / TILE_SIZE) + 1;
    if (closestX < 0) closestX = 0;
    if (furthestX >= WORLD_W) furthestX = WORLD_W - 1;
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = closestX; x <= furthestX; x++) {
            int tileID = worldMap[y][x];
            if (tileID > 0 && tileID < TOTAL_BLOCKS) {
                Texture2D blockTexture = blocks[tileID].sprite;
                if (tileID == 6 || tileID == 7) {
                    blockTexture = redTileOn ? blocks[6].sprite : blocks[7].sprite;
                }
                if (tileID == 8 || tileID == 9) {
                    blockTexture = redTileOn ? blocks[9].sprite : blocks[8].sprite;
                }
                DrawTexture(blockTexture, x * 8, y * 8 , WHITE); 
            }
        }
    }
}

void updateCycleTiles() {
    int cycleLength = 60;
    static int cycleFrame = 0;
    cycleFrame++;
    if (cycleFrame >= cycleLength) {
        cycleFrame = 0;
        redTileOn = !redTileOn;
    }
}

bool tileSolid(int x, int y) {
    if (x / TILE_SIZE < 0 || x / TILE_SIZE >= WORLD_W || y / TILE_SIZE < 0 || y / TILE_SIZE >= WORLD_H) return true;
    int tileID = worldMap[y / TILE_SIZE][x / TILE_SIZE];
    if (tileID == 6 || tileID == 7) return redTileOn;
    if (tileID == 8 || tileID == 9) return !redTileOn;
    return blocks[tileID].solid;
}