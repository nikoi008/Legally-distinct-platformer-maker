#include "editor.h"
#include "blocks.h"
#include "raymath.h"
#include <math.h>

bool rectangle = false;
bool editing = true;

bool mouseOverUi() {
    return (virtualMouse.y >= SCREEN_H - 32);
}
bool drawbuttonTopRight(Texture2D sprite){
    float x = (float)(SCREEN_W - sprite.width - 5);
    float y = (float)5;

    Rectangle spriteRec = { x, y, (float)sprite.width, (float)sprite.height };
    bool hovered = CheckCollisionPointRec(virtualMouse, spriteRec);
    DrawTexture(sprite, (int)x, (int)y, hovered ? Fade(WHITE, 0.6f) : WHITE);
    return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}
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

void tileSelect() {
    int blocksDrawn = 0;
    DrawRectangle(0, SCREEN_H - 32, SCREEN_W, 32, GRAY);
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        float xPos = 8.0f + (blocksDrawn * 12.0f);
        float yPos = SCREEN_H - 24.0f;
        Rectangle tileHitbox = { xPos, yPos, 8, 8 };
        if (!blocks[i].showInEdtior) {
            continue;
        }
        DrawTexture(blocks[i].sprite, xPos, yPos, WHITE);
        if (currentTile == blocks[i].blockID)
            DrawRectangleLinesEx((Rectangle) { xPos - 1, yPos - 1, 10, 10 }, 1, WHITE);
        if (CheckCollisionPointRec(virtualMouse, tileHitbox)) {
            DrawRectangleLinesEx(tileHitbox, 1, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentTile = blocks[i].blockID;
            }
        }
        blocksDrawn++;
        if( i ==0){
            DrawTexture(trash,xPos,yPos,WHITE);
        }
    }
}

void rectangleMode(Vector2 firstTilePos) {
    Vector2 worldMouse = GetScreenToWorld2D(virtualMouse, camera);
    int currentTileX = (int)(worldMouse.x / TILE_SIZE);
    int currentTileY = (int)(worldMouse.y / TILE_SIZE);

    int topLeftX = (int)fminf(firstTilePos.x, (float)currentTileX);
    int topLeftY = (int)fminf(firstTilePos.y, (float)currentTileY);
    int bottomLeftX = (int)fmaxf(firstTilePos.x, (float)currentTileX);
    int bottomLeftY = (int)fmaxf(firstTilePos.y, (float)currentTileY);
    int width = (bottomLeftX - topLeftX + 1) * TILE_SIZE;
    int height = (bottomLeftY - topLeftY + 1) * TILE_SIZE;
    DrawRectangle(topLeftX * 8, topLeftY * 8, width, height, Fade(GREEN, 0.3f));
    DrawRectangleLinesEx((Rectangle) { (float)(topLeftX * 8), (float)(topLeftY * 8), (float)width, (float)height }, 1, GREEN);
}

void fillRectangle(Vector2 startPoint, Vector2 endPoint, int tileId) {
    int xStart = fminf(startPoint.x, endPoint.x);
    int yStart = fminf(startPoint.y, endPoint.y);
    int xEnd = fmaxf(startPoint.x, endPoint.x);
    int yEnd = fmaxf(startPoint.y, endPoint.y);
    for (int y = yStart; y < yEnd + 1; y++) {
        for (int x = xStart; x < xEnd + 1; x++) {
            if (x >= 0 && x < WORLD_W && y >= 0 && y < WORLD_H) {
                placeTile(x, y, tileId);
            }
        }
    }
}

bool editorFrame(Texture2D flagWarning) {
    static bool initialMouse = false;
    static Vector2 firstClickPos = { 0,0 };
    static int warningFrames = 0;

    BeginMode2D(camera);
    
    for (int x = 0; x <= WORLD_W; x++)
        DrawLine(x * TILE_SIZE, 0, x * TILE_SIZE, WORLD_H * TILE_SIZE, GRAY);
    for (int y = 0; y <= WORLD_H; y++)
        DrawLine(0, y * TILE_SIZE, WORLD_W * TILE_SIZE, y * TILE_SIZE, GRAY);

    Vector2 worldMouse = GetScreenToWorld2D(virtualMouse, camera);
    int mapX = (int)(worldMouse.x / TILE_SIZE);
    int mapY = (int)(worldMouse.y / TILE_SIZE);

    if (mapX >= 0 && mapX < WORLD_W && mapY >= 0 && mapY < WORLD_H && !mouseOverUi()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) && !rectangle) {
            placeTile(mapX, mapY, currentTile);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            worldMap[mapY][mapX] = 0;
        }

        if (IsKeyPressed(KEY_R)) {
            rectangle = !rectangle;
            initialMouse = false;
        }

        if (rectangle) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (!initialMouse) {
                    firstClickPos = (Vector2){ (float)mapX, (float)mapY };
                    initialMouse = true;
                }
                rectangleMode(firstClickPos);
            }
            else if (initialMouse) {
                fillRectangle(firstClickPos, (Vector2) { (float)mapX, (float)mapY }, currentTile);
                initialMouse = false;
            }
        }
        DrawRectangleLines(mapX * TILE_SIZE, mapY * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
    } else {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            initialMouse = false;
        }
    }

    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            int tileID = worldMap[y][x];
            if (tileID > 0 && tileID < TOTAL_BLOCKS) {
                DrawTexture(blocks[tileID].sprite, x * TILE_SIZE, y * TILE_SIZE, WHITE);
            }
        }
    }
    EndMode2D();
    if (IsKeyPressed(KEY_P) || drawbuttonTopRight(play)) {
        if (flagPos.x != -1) {
            editing = false;
        }
        else {
            warningFrames = 180;
        }
    }
    BeginMode2D(camera);

    if (warningFrames > 0) {
        EndMode2D();
        DrawTexture(flagWarning, SCREEN_W - flagWarning.width, 0, WHITE);
        warningFrames--;
        BeginMode2D(camera);
    }


    EndMode2D();
    return editing;
}