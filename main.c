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

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "Legally distinct mario maker");
    SetTargetFPS(60);
    initBlocks();
    InitAudioDevice();
    currentState = MENU;
    camera.zoom = 1.0f;
    RenderTexture2D window = LoadRenderTexture(SCREEN_W, SCREEN_H);
    player.pTex = LoadTexture("sprites/player/player.png");
    Texture2D flagWarning = LoadTexture("sprites/menu/flag_warning.png");
    coinPickup = LoadSound("audio/pickupCoin.wav");
    Texture2D bg = LoadTexture("sprites/menu/bg.png");
    SetTextureFilter(bg, TEXTURE_FILTER_POINT);
    SetTextureFilter(window.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
        float scale = fminf((float)GetScreenWidth() / SCREEN_W, (float)GetScreenHeight() / SCREEN_H);
        Vector2 mouse = GetMousePosition();
        virtualMouse = (Vector2){
            (mouse.x - (GetScreenWidth() - (SCREEN_W * scale)) * 0.5f) / scale,
            (mouse.y - (GetScreenHeight() - (SCREEN_H * scale)) * 0.5f) / scale
        };

        if (currentState == EDITOR) cameraInput();

        BeginTextureMode(window);
        ClearBackground(RAYWHITE);
        switch (currentState) {
        case(MENU):
            DrawText("Menu", 110, 110, 10, DARKGRAY);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = EDITOR;
            drawBG(bg);
            break;
        case(EDITOR):
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
            if (IsKeyPressed(KEY_E)) {
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