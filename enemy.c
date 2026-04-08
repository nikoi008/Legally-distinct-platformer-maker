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