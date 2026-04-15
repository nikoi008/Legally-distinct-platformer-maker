#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "blocks.h"
float important_hedgehog_meaning_of_life( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       
	i  = 0x5f3759df - ( i >> 1 );               
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   


	return y;
}
enemyHedgehog* hedgehogs = NULL;
int hedgehogCount = 0;
int currentMaxHedgehog = 0;
void addHedgehog(float x, float y){
	if(hedgehogCount >= currentMaxHedgehog){
		currentMaxHedgehog = (currentMaxHedgehog == 0) ? 4 : currentMaxHedgehog * 2;
		hedgehogs = realloc(hedgehogs,sizeof(enemyHedgehog) * currentMaxHedgehog);
	}
	hedgehogs[hedgehogCount].x = x;
    hedgehogs[hedgehogCount].y = y;
    hedgehogs[hedgehogCount].left = true;
    hedgehogs[hedgehogCount].active = true;
    hedgehogCount++;
}

void clearHedgehog(){
	if(hedgehogs != NULL){
		free(hedgehogs);
		hedgehogs = NULL;
	}
	hedgehogCount = 0;
	currentMaxHedgehog = 0;
}
void updateHedgehog(){
    for (int i = 0; i < hedgehogCount; i++) {
        if (!hedgehogs[i].active) continue;

        float moveDir = hedgehogs[i].left ? -HEDGEHOG_SPEED : HEDGEHOG_SPEED;
        float hedgeX = hedgehogs[i].x + moveDir;

        if (tileSolid(hedgeX + (hedgehogs[i].left ? 0 : 7), hedgehogs[i].y + 4)) {
            hedgehogs[i].left = !hedgehogs[i].left;
        } else {
            hedgehogs[i].x = hedgeX;
        }
        if (!tileSolid(hedgehogs[i].x, hedgehogs[i].y + 8) && !tileSolid(hedgehogs[i].x + 7, hedgehogs[i].y + 8)) {
            hedgehogs[i].y += 1.0f;
        }
        Rectangle hedgehogRect = { hedgehogs[i].x, hedgehogs[i].y, 8, 8 };
        Rectangle playerRect = { player.x, player.y, PLAYER_HITBOX, PLAYER_HITBOX };
        if (CheckCollisionRecs(hedgehogRect, playerRect)) {
            dieScreen();
        }
    }
}
void drawHedgehog(){
    for (int i = 0; i < hedgehogCount; i++) {
        if (hedgehogs[i].active) {
            Rectangle source = { 0.0f,0.0f, (hedgehogs[i].left ? (float)blocks[11].sprite.width :  -(float)blocks[11].sprite.width),(float)blocks[11].sprite.height};
            Rectangle dest = { hedgehogs[i].x,hedgehogs[i].y,(float)blocks[11].sprite.width,(float)blocks[11].sprite.height};
            DrawTexturePro(blocks[11].sprite, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }
    }
}
movingPad* pads = NULL;
int padCount = 0;
int currentMaxPad = 0;

void addPad(float x, float y) {
    if (padCount >= currentMaxPad) {
        currentMaxPad = (currentMaxPad == 0) ? 4 : currentMaxPad * 2;
        pads = realloc(pads, sizeof(movingPad) * currentMaxPad);
    }
    pads[padCount].x = x;
    pads[padCount].y = y;
    pads[padCount].left = true;
    padCount++;
}

void clearPad() {
    if (pads != NULL) {
        free(pads);
        pads = NULL;
    }
    padCount = 0;
    currentMaxPad = 0;
}
void updatePad() {
    for (int i = 0; i < padCount; i++) {
        float moveDir = pads[i].left ? -HEDGEHOG_SPEED : HEDGEHOG_SPEED;
        float newX = pads[i].x + moveDir;
        float checkX = pads[i].left ? newX : newX + TILE_SIZE - 1;

        bool blocked = tileSolid(checkX, pads[i].y + 4);
        if (!blocked) {
            for (int j = 0; j < padCount; j++) {
                if (j == i) continue;
                if (checkX >= pads[j].x && checkX < pads[j].x + TILE_SIZE &&
                    pads[i].y + 4 >= pads[j].y && pads[i].y + 4 < pads[j].y + TILE_SIZE)
                    blocked = true;
            }
        }

        if (blocked) {
            pads[i].left = !pads[i].left;
        } else {
            float padTop = pads[i].y;
            float playerBottom = player.y + PLAYER_HITBOX;
            bool playerAbove = playerBottom >= padTop - 1 && playerBottom <= padTop + 2;
            bool playerOverlapsX = player.x + PLAYER_HITBOX > pads[i].x && player.x < pads[i].x + TILE_SIZE;
            if (playerAbove && playerOverlapsX && player.onGround)
                player.x += moveDir;
            pads[i].x = newX;
        }
    }
}

void drawPad() {
    for (int i = 0; i < padCount; i++) {
        DrawTexture(blocks[12].sprite, (int)pads[i].x, (int)pads[i].y, WHITE);
    }
}
bool padSolid(float x, float y) {
    for (int i = 0; i < padCount; i++) {
        if (x >= pads[i].x && x < pads[i].x + TILE_SIZE &&
            y >= pads[i].y && y < pads[i].y + TILE_SIZE)
            return true;
    }
    return false;
}