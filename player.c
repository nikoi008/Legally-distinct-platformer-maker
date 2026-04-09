#include "player.h"
#include "blocks.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include "enemy.h"
#define GRAVITY 0.5f
#define JUMP_FORCE 5.0f
#define PLAYER_SIZE 7
#define COYOTE_FRAMES 6

playe player = { 0 };
int coyoteFrame = 0;

void initPlayer() {
    SetTextureFilter(player.pTex, TEXTURE_FILTER_POINT);
    camera.offset = (Vector2){ SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    camera.zoom = 1.0f;
    player.x = flagPos.x * 8;
    player.y = flagPos.y * 8;
    camera.target = (Vector2){ player.x + 4,player.y + 4 };
    player.tileX = flagPos.x;
    player.tileY = flagPos.y;
    player.acceleration = 0.2f;
    player.maxSpeed = 1.75f;
    player.currentSpeed = 0.0f;
    //player.jumping = false;
    //player.falling = false;
    //player.sprinting = false; remove?
    player.powerup = 0; //implement in future?
    player.facingRight = false;
    player.coins = 0;
    clearHedgehog();
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            if (worldMap[y][x] == 11) {
                addHedgehog(x * TILE_SIZE, y * TILE_SIZE); //todo make it reset on screen when playing but keep in editor
                worldMap[y][x] = 0; 
            }
        }
    }
}

void winScreen() {
    currentState = EDITOR;
    editing = true;
}

void dieScreen() {
    player.x = flagPos.x * 8;
    player.y = flagPos.y * 8;
    player.currentSpeed = 0;
}

int coordsToTile(int coord) {
    return (int)coord / 8;
}

void callIfTouched() {
    int left = coordsToTile(player.x);
    int right = coordsToTile(player.x + PLAYER_HITBOX - 1);
    int top = coordsToTile(player.y);
    int bottom = coordsToTile(player.y + PLAYER_HITBOX - 1);

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            if (x < 0 || x >= WORLD_W || y < 0 || y >= WORLD_H) {
                dieScreen();
                return;
            }

            int tileID = worldMap[y][x];
            if (blocks[tileID].ifTouched != NULL) {
                player.tileX = x;
                player.tileY = y;
                blocks[tileID].ifTouched();
            }
        }
    }
}
 
bool checkCollision(float x, float y) {
    int left = (int)x / TILE_SIZE;
    int right = (int)(x + PLAYER_SIZE - 1) / TILE_SIZE;
    int top = (int)y / TILE_SIZE;
    int bottom = (int)(y + PLAYER_SIZE - 1) / TILE_SIZE;

    if (left < 0 || right >= WORLD_W || top < 0 || bottom >= WORLD_H) return true;
    return tileSolid(left * TILE_SIZE, top * TILE_SIZE) ||
        tileSolid(right * TILE_SIZE, top * TILE_SIZE) ||
        tileSolid(left * TILE_SIZE, bottom * TILE_SIZE) ||
        tileSolid(right * TILE_SIZE, bottom * TILE_SIZE);
}
#define PLAYER_FRITCION 0.1f

void playerInput() {
    bool moving = false;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player.currentSpeed -= player.acceleration;
        player.facingRight = false;
        moving = true;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player.currentSpeed += player.acceleration;
        player.facingRight = true;
        moving = true;
    }
    if (!moving) {
        if (player.currentSpeed > 0) {
            player.currentSpeed -=PLAYER_FRITCION;
            if (player.currentSpeed < 0) player.currentSpeed = 0; 
        }
        else if (player.currentSpeed < 0) {
            player.currentSpeed += PLAYER_FRITCION;
            if(player.currentSpeed > 0) player.currentSpeed = 0;    
        }
    }
    if (IsKeyPressed(KEY_SPACE)) {
        if (player.onGround || coyoteFrame < COYOTE_FRAMES) {
            player.speedY = -JUMP_FORCE;
            player.onGround = false;
            coyoteFrame = COYOTE_FRAMES;
            //moving???
        }
    }
    player.currentSpeed = Clamp(player.currentSpeed, -player.maxSpeed, player.maxSpeed);
    
}

void playerPhysics() {
    player.x += player.currentSpeed;
    if (checkCollision(player.x, player.y)) {
        if (player.currentSpeed > 0)
            player.x = (floorf((player.x + PLAYER_SIZE - 1) / TILE_SIZE) * TILE_SIZE) - PLAYER_SIZE;
        else if (player.currentSpeed < 0)
            player.x = (floorf(player.x / TILE_SIZE) + 1) * TILE_SIZE;
        player.currentSpeed = 0;
    }

    player.speedY += GRAVITY;
    player.y += player.speedY;

    player.onGround = false;
    if (checkCollision(player.x, player.y)) {
        if (player.speedY > 0) {
            player.y = (floorf((player.y + PLAYER_SIZE - 1) / TILE_SIZE) * TILE_SIZE) - PLAYER_SIZE;
            player.onGround = true;
            coyoteFrame = 0;
        }
        else if (player.speedY < 0) {
            player.y = (floorf(player.y / TILE_SIZE) + 1) * TILE_SIZE;
        }
        player.speedY = 0;
    }
    else {
        coyoteFrame++;
    }
    //player.currentSpeed *= 0.93f;
}

void drawPlayer() {
    Rectangle source = { 0.0f,0.0,(player.facingRight ? (float)player.pTex.width : -(float)player.pTex.width), (float)player.pTex.height };
    Rectangle dest = { player.x,player.y - 1,(float)player.pTex.width,(float)(player.pTex.height)};
    DrawTexturePro(player.pTex, source, dest, (Vector2){ 0.0f,0.0f }, 0.0f, WHITE);
}
void playerCamera() {
    Vector2 targetPos = (Vector2){ player.x + 4, player.y + 4 };
    camera.target = Vector2Lerp(camera.target, targetPos, 0.1f);
}