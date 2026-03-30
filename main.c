#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define SCREEN_W 320
#define SCREEN_H 240
#define WORLD_W 300
#define WORLD_H 50
#define TILE_SIZE 8

typedef enum { MENU, EDITOR, PLATFORMER} GameState;

unsigned char worldMap[WORLD_H][WORLD_W] = { 0 }; 
Camera2D camera = { 0 };
Vector2 virtualMouse = { 0 };

void cameraInput() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(virtualMouse, camera);
        camera.offset = virtualMouse;
        camera.target = mouseWorldPos;
        camera.zoom = Clamp(camera.zoom + wheel * 0.1f, 0.33f, 3.0f);
    }
}  

typedef struct
{
    Texture2D sprite;
    int blockID;
    bool solid; 
    bool coyoteTime; 
    bool isUnique;
}block;

block blocks[4] = {0};
Vector2 flagPos = {-1,-1};
#define TOTAL_BLOCKS 4
int currentTile = 1;

void tileSelect() {
    DrawRectangle(0, SCREEN_H - 32, SCREEN_W, 32, GRAY);
    for (int i = 0; i < TOTAL_BLOCKS; i++) { 
        float xPos = 8.0f + (i * 12.0f);
        float yPos = SCREEN_H - 24.0f;
        Rectangle tileHitbox = { xPos, yPos, 8, 8 };        
        DrawTexture(blocks[i].sprite, xPos, yPos, WHITE);
        if (currentTile == blocks[i].blockID) 
            DrawRectangleLinesEx((Rectangle){xPos-1, yPos-1, 10, 10}, 1, WHITE);
        if (CheckCollisionPointRec(virtualMouse, tileHitbox)) {
            DrawRectangleLinesEx(tileHitbox, 1, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentTile = blocks[i].blockID;
            }
        }
    }
}

void initBlocks(){
    Texture2D air = LoadTexture("air.png");
    blocks[0].sprite = air;    
    blocks[0].blockID = 0;
    blocks[0].solid = false;
    blocks[0].isUnique = false;

    Texture2D ground = LoadTexture("ground.png");       
    blocks[1].sprite = ground;  
    blocks[1].blockID = 1;
    blocks[1].solid = true;
    blocks[1].coyoteTime = true;
    blocks[1].isUnique = false;

    Texture2D brick = LoadTexture("brick.png");
    blocks[2].sprite = brick;
    blocks[2].blockID = 2;
    blocks[2].solid = true;
    blocks[2].coyoteTime = true;
    blocks[2].isUnique = false;
    
    Texture2D flag = LoadTexture("flag.png");
    blocks[3].sprite = flag;
    blocks[3].blockID = 3;
    blocks[3].solid = false;
    blocks[3].coyoteTime = false;
    blocks[3].isUnique = true;
    for(int i   =0; i < TOTAL_BLOCKS; i++){
        SetTextureFilter(blocks[i].sprite, TEXTURE_FILTER_POINT);
    }
}

void placeTile(int tileX, int tileY, int tileID){
        if(blocks[tileID].isUnique == true){
            switch(tileID){
                case 3:
                    if(flagPos.x != -1){
                        worldMap[(int)flagPos.y][(int)flagPos.x] = 0;
                    }
                    flagPos.x = tileX;
                    flagPos.y = tileY;
                    break;
            }
        }
        worldMap[tileY][tileX] = tileID;
}

bool editing = true;
bool editorFrame(Texture2D flagWarning) {
    BeginMode2D(camera);
    static int warningFrames = 0;
        for (int x = 0; x <= WORLD_W; x++) 
            DrawLine(x * TILE_SIZE, 0, x * TILE_SIZE, WORLD_H * TILE_SIZE, GRAY);
        for (int y = 0; y <= WORLD_H; y++) 
            DrawLine(0, y * TILE_SIZE, WORLD_W * TILE_SIZE, y * TILE_SIZE, GRAY);
        Vector2 worldMouse = GetScreenToWorld2D(virtualMouse, camera);
        int mapX = (int)(worldMouse.x / TILE_SIZE);
        int mapY = (int)(worldMouse.y / TILE_SIZE);

        if (mapX >= 0 && mapX < WORLD_W && mapY >= 0 && mapY < WORLD_H) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) && virtualMouse.y < SCREEN_H - 32) placeTile(mapX,mapY,currentTile);
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) worldMap[mapY][mapX] = 0;  
            DrawRectangleLines(mapX * TILE_SIZE, mapY * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE); 
        }

        for (int y = 0; y < WORLD_H; y++) {
            for (int x = 0; x < WORLD_W; x++) {
                int tileID = worldMap[y][x];
                if (tileID > 0 && tileID < TOTAL_BLOCKS) {
                    DrawTexture(blocks[tileID].sprite, x * TILE_SIZE, y * TILE_SIZE, WHITE);
                }
            }
        }
        if(IsKeyPressed(KEY_P)){
            if(flagPos.x != -1){
                editing = false;
            }else{
                warningFrames = 180;
            }
        }
        if(warningFrames > 0){
            EndMode2D();
            DrawTexture(flagWarning,SCREEN_W-flagWarning.width,0,WHITE);
            warningFrames--;
            BeginMode2D(camera);
        }
    EndMode2D();
    return editing;
}

void DrawLevel(){
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            int tileID = worldMap[y][x];
            if (tileID > 0 && tileID < TOTAL_BLOCKS) {
                DrawTexture(blocks[tileID].sprite, x * TILE_SIZE, y * TILE_SIZE, WHITE);
            }
        }
    }
}
typedef struct{
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
} playe;
playe player = {0};

void initPlayer(){
    SetTextureFilter(player.pTex, TEXTURE_FILTER_POINT);
    camera.offset = (Vector2){ SCREEN_W/2.0f, SCREEN_H/2.0f }; 
    camera.zoom = 1.0f;
    player.x = flagPos.x * 8;
    player.y = flagPos.y * 8;
    camera.target = (Vector2){player.x + 4,player.y + 4};
    player.tileX = flagPos.x;
    player.tileY = flagPos.y;
    player.acceleration = 0.2f;
    player.maxSpeed = 3.0f;
    player.currentSpeed = 0.0f;
    player.jumping = false;
    player.falling = false;
    player.sprinting = false;
    player.powerup = 0;
    player.facingRight = false;
}

#define GRAVITY 0.5f
#define JUMP_FORCE 5.0f

bool tileSolid(int x, int y){
    if(x / TILE_SIZE < 0 || x / TILE_SIZE >= WORLD_W || y / TILE_SIZE < 0 || y / TILE_SIZE >= WORLD_H) return true;
    return blocks[worldMap[y /8][x /8]].solid;
}

void playerCamera(){
    Vector2 targetPos = (Vector2){ player.x + 4, player.y + 4}; 
    camera.target = Vector2Lerp(camera.target, targetPos, 0.1f);
}

int coordsToTile(int coord){
    return coord / 8;
}

void playerInput(){
    bool moving = false;  
    if(IsKeyDown(KEY_LEFT) && !tileSolid(player.x - 1, player.y)){ 
        player.currentSpeed -= player.acceleration;
        player.facingRight = false;
        moving = true;
    }

    if(IsKeyDown(KEY_RIGHT) && !tileSolid(player.x + TILE_SIZE,player.y)){
        player.currentSpeed += player.acceleration;
        player.facingRight = true;
        moving = true;
    }
    player.currentSpeed = Clamp(player.currentSpeed,-player.maxSpeed,player.maxSpeed);
    player.x += (int)player.currentSpeed;
    if(IsKeyDown(KEY_SPACE) && player.onGround){
        player.speedY = -JUMP_FORCE;
        player.onGround = false;
    }
}

void playerPhysics(){
    player.speedY += GRAVITY;
    player.y += player.speedY;
    if(tileSolid(player.x,player.y + TILE_SIZE)){
        player.y = (player.y / TILE_SIZE) * TILE_SIZE;
        player.speedY = 0;
        player.onGround = true;
    }
    else{
        player.onGround = false;
    }
}

void drawPlayer() {
    Rectangle source = {0.0f,0.0f,(player.facingRight ? (float)player.pTex.width : -(float)player.pTex.width), (float)player.pTex.height};
    Rectangle dest = {player.x,player.y,(float)player.pTex.width,(float)(player.pTex.height)};
    DrawTexturePro(player.pTex, source, dest, (Vector2){0.0f,0.0f}, 0.0f, WHITE);
}

void platformerFrame(){
    BeginMode2D(camera);
    DrawLevel();
    player.tileX = coordsToTile(player.x);
    player.tileY = coordsToTile(player.y);
    playerPhysics();
    playerInput();
    drawPlayer();
    playerCamera();
    EndMode2D();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "Legally distinct mario maker");       
    SetTargetFPS(60);
    initBlocks();
    GameState currentState = MENU;             
    camera.zoom = 1.0f;
    RenderTexture2D window = LoadRenderTexture(SCREEN_W, SCREEN_H);
    player.pTex = LoadTexture("player.png");
    Texture2D flagWarning = LoadTexture("flag_warning.png");

    SetTextureFilter(window.texture, TEXTURE_FILTER_POINT);
    while (!WindowShouldClose()) {
        float scale = fminf((float)GetScreenWidth()/SCREEN_W, (float)GetScreenHeight()/SCREEN_H);
        Vector2 mouse = GetMousePosition();
        virtualMouse = (Vector2){
            (mouse.x - (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f) / scale,
            (mouse.y - (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f) / scale
        };

        if (currentState == EDITOR) cameraInput(); 

        BeginTextureMode(window);
            ClearBackground(RAYWHITE);
            switch(currentState){
                case(MENU):
                    DrawText("Menu", 110, 110, 10, DARKGRAY); 
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = EDITOR;
                    break;
                case(EDITOR):
                    if(editorFrame(flagWarning)){
                        tileSelect();
                    }else{
                        initPlayer();
                        currentState = PLATFORMER;
                    }
                    break;
                case(PLATFORMER):
                    platformerFrame();
                    if(IsKeyPressed(KEY_E)){
                        currentState = EDITOR;
                        flagPos = (Vector2){-1,-1};
                        editing = true;
                    }
                    break;
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            Rectangle source = { 0, 0, (float)window.texture.width, (float)-window.texture.height };
            Rectangle dest = { (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f, (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f, SCREEN_W * scale, SCREEN_H * scale };
            DrawTexturePro(window.texture, source, dest, (Vector2){0,0}, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(window);
    CloseWindow();
    return 0; 
}