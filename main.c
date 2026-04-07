#include "globals.h"
#include "blocks.h"
#include "player.h"
#include "editor.h"
#include <stdio.h>
#include <math.h>
#include "enemy.h"
GameState currentState = MENU;
Camera2D camera = { 0 };
Vector2 virtualMouse = { 0 };
Sound coinPickup;

void drawBG(Texture2D tBackground) {
    static int scrollX;
    static int scrollY;
    const int speed = 1;
    scrollX -= speed;
    scrollY -= speed;
    if (scrollX <= -SCREEN_W) scrollX += SCREEN_W;
    if (scrollY <= -SCREEN_H) scrollY += SCREEN_H;
    DrawTexture(tBackground, (int)scrollX, (int)scrollY, WHITE);
    DrawTexture(tBackground, (int)scrollX + SCREEN_W, (int)scrollY, WHITE);
    DrawTexture(tBackground, (int)scrollX, (int)scrollY + SCREEN_H, WHITE);
    DrawTexture(tBackground, (int)scrollX + SCREEN_W, (int)scrollY + SCREEN_H, WHITE);
}
void drawTitle(Texture2D title) {
    int x = (SCREEN_W / 2) - (title.width / 2);
    int y = (SCREEN_H / 4) - (title.height / 2);
    DrawTexture(title, x, y, WHITE);
}
bool drawStart(Texture2D start) {
    Rectangle bounds = { (float)(SCREEN_W / 2) - (156 / 2), (float)130, (float)156, (float)45 };
    bool hovered = CheckCollisionPointRec(virtualMouse, bounds);
    DrawRectangleRec(bounds, Fade(WHITE,0.0f));
    int texX = (SCREEN_W / 2) - (156 / 2) + (156 / 2) - (start.width / 2);
    int texY = 130 + (45 / 2) - (start.height / 2);
    DrawTexture(start, texX, texY, hovered ? Fade(BLACK,0.3f) : Fade(WHITE,1.0f));
    return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
    
}

void platformerFrame() {
    BeginMode2D(camera);
    DrawLevel();
    updateHedgehog();
    drawHedgehog();
    player.tileX = coordsToTile(player.x);
    player.tileY = coordsToTile(player.y);
    playerPhysics();
    playerInput();
    callIfTouched();
    updateCycleTiles();
    drawPlayer();
    playerCamera();
    EndMode2D();
}
Texture2D play;
Texture2D trash;
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "Legally distinct mario maker");
    SetTargetFPS(60);
    initBlocks();
    InitAudioDevice();
    currentState = MENU;
    camera.zoom = 1.0f;
    Texture2D editButton = LoadTexture("sprites/menu/edit.png");
    play = LoadTexture("sprites/menu/play.png");
    RenderTexture2D window = LoadRenderTexture(SCREEN_W, SCREEN_H);
    player.pTex = LoadTexture("sprites/player/player.png");
    Texture2D flagWarning = LoadTexture("sprites/menu/flag_warning.png");
    coinPickup = LoadSound("audio/pickupCoin.wav");
    Texture2D bg = LoadTexture("sprites/menu/bg.png");
    Texture2D title = LoadTexture("sprites/menu/title.png");
    SetTextureFilter(bg, TEXTURE_FILTER_POINT);
    SetTextureFilter(window.texture, TEXTURE_FILTER_POINT);
    Texture2D start = LoadTexture("sprites/menu/start.png");
    trash = LoadTexture("sprites/blocks/trash.png");
    while (!WindowShouldClose()) {
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, (float)GetScreenHeight() / SCREEN_H);
        Vector2 mouse = GetMousePosition();
        virtualMouse = (Vector2){
            (mouse.x - (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f) / scale,
            (mouse.y - (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f) / scale
        };

        

        BeginTextureMode(window);
        ClearBackground(RAYWHITE);
        switch (currentState) {
        case(MENU):
            DrawText("Menu", 110, 110, 10, DARKGRAY);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = EDITOR;
            drawBG(bg);
            drawTitle(title);
            if (drawStart(start)) { 
                currentState = EDITOR;
            }
            break;
        case(EDITOR):
            cameraInput();
            if (editorFrame(flagWarning)) {
                tileSelect();
            }
            else {
                initPlayer();
                currentState = PLATFORMER;
            }
            break;
        case(PLATFORMER):
            platformerFrame();
            if (IsKeyPressed(KEY_E) || drawbuttonTopRight(editButton)) {
                currentState = EDITOR;
                editing = true;
            }
            break;
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle source = { 0, 0, (float)window.texture.width, (float)-window.texture.height };
        Rectangle dest = { (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f, (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f, SCREEN_W * scale, SCREEN_H * scale };
        DrawTexturePro(window.texture, source, dest, (Vector2){ 0,0 }, 0.0f, WHITE);
        EndDrawing();
    }
    CloseAudioDevice();
    UnloadRenderTexture(window);
    CloseWindow();
    return 0;
}