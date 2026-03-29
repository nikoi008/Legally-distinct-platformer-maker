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

unsigned char worldMap[WORLD_W * WORLD_H] = { 0 }; 
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

}
void placeTile(int tileX, int tileY, int tileID){
        if(blocks[tileID].isUnique == true){
            switch(tileID){
                case 3:
                    if(flagPos.x != -1){
                        worldMap[(int)flagPos.y * WORLD_W + (int)(flagPos.x)] = 0;
                    }
                    flagPos.x = tileX;
                    flagPos.y = tileY;
                    break;
            }
        }
        worldMap[tileY * WORLD_W + tileX] = tileID;
        
}
bool editing = true;
bool editorFrame() {
    BeginMode2D(camera);
        printf("flag X %f",flagPos.x);
        printf("flag Y %f",flagPos.y);
        
        for (int x = 0; x <= WORLD_W; x++) 
            DrawLine(x * TILE_SIZE, 0, x * TILE_SIZE, WORLD_H * TILE_SIZE, GRAY);
        for (int y = 0; y <= WORLD_H; y++) 
            DrawLine(0, y * TILE_SIZE, WORLD_W * TILE_SIZE, y * TILE_SIZE, GRAY);
        Vector2 worldMouse = GetScreenToWorld2D(virtualMouse, camera);
        int mapX = (int)(worldMouse.x / TILE_SIZE);
        int mapY = (int)(worldMouse.y / TILE_SIZE);

        if (mapX >= 0 && mapX < WORLD_W && mapY >= 0 && mapY < WORLD_H) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) && virtualMouse.y < SCREEN_H - 32) /*worldMap[mapY * WORLD_W + mapX] = currentTile*/ placeTile(mapX,mapY,currentTile);
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) worldMap[mapY * WORLD_W + mapX] = 0;  
            DrawRectangleLines(mapX * TILE_SIZE, mapY * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE); 
            
        }
        for (int y = 0; y < WORLD_H; y++) {
            for (int x = 0; x < WORLD_W; x++) {
                switch(worldMap[y * WORLD_W + x]){
                case 0:
                    //DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE - 2, TILE_SIZE - 2, WHITE);
                    break;
                case 1:
                    DrawTexture(blocks[1].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                    break;
                case 2:
                    DrawTexture(blocks[2].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                    break;  
                case 3:
                    DrawTexture(blocks[3].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                    break;
                
                }
            }
        }
        if(IsKeyPressed(KEY_P)) editing = false;

        
    EndMode2D();
    return editing;
}
void DrawLevel(){
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            switch(worldMap[y * WORLD_W + x]){
            case 0:
                //DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE - 2, TILE_SIZE - 2, WHITE);
                break;
            case 1:
                DrawTexture(blocks[1].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                break;
            case 2:
                DrawTexture(blocks[2].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                break;  
            case 3:
                DrawTexture(blocks[3].sprite, x * TILE_SIZE, y * TILE_SIZE,WHITE);
                break;
            
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
    int acceleration;
    int maxSpeed;
    bool jumping;
    bool falling;
    bool sprinting;
    int powerup;
    bool facingRight;

} playe;
playe player = {0};
void initPlayer(){
   player.x = flagPos.x * 8;
   player.y = flagPos.y * 8;
   player.tileX = flagPos.x;
   player.tileY = flagPos.y;
   player.acceleration = 0;
   player.maxSpeed = 5;
   player.jumping = false;
   player.falling = false;
   player.sprinting = false;
   player.powerup = 0;
   player.facingRight = false;
}

void coordsToTile(){
    player.tileX = player.x / 8;
    player.tileY = player.y / 8;
}
void playerInput(){
    if(IsKeyDown(KEY_LEFT)){
        player.x--;
        player.facingRight = false;
    }
    if(IsKeyDown(KEY_RIGHT)){
        player.x++;
        player.facingRight = true;
    }
    if(IsKeyDown(KEY_SPACE)){
        
    }
}
void drawPlayer() {
    DrawTexture(player.pTex,player.x,player.y,WHITE);
}
void platformerFrame(){
    DrawLevel();
    coordsToTile();
    
    playerInput();
    drawPlayer();
   // DrawText("its a me",100,100,10,BLACK);

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
                    if(editorFrame()){
                        tileSelect();
                    }else{
                        initPlayer();
                        currentState = PLATFORMER;
                    }
                    break;
                case(PLATFORMER):
                    platformerFrame();

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