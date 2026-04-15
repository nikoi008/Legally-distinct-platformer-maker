#include "editor.h"
#include "blocks.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
bool rectangle = false;
bool isPanning = false; 
bool editing = true;

typedef struct {
    char* description;
    char* shortcut;
    Rectangle bounds;
    Texture2D sprite;
} editorTooltips;

#define SHORTCUTS 2
editorTooltips tooltips[TOTAL_BLOCKS + SHORTCUTS] = {
    [0]  = {"Deletes the selected block","Air/Delete",{8,SCREEN_H-24,8,8},{0}},
    [1]  = {"A solid block on all 4 sides","Grass",{20,SCREEN_H-24,8,8},{0}},
    [2]  = {"A solid block on all 4 sides","Brick",{32,SCREEN_H-24,8,8},{0}},
    [3]  = {"Determines the player's start position","Start Flag",{44,SCREEN_H-24,8,8},{0}},
    [4]  = {"Completes the level when touched","End Flag",{56,SCREEN_H-24,8,8},{0}},
    [5]  = {"Returns player to start when touched","Spike",{68,SCREEN_H-24,8,8},{0}},
    [6]  = {"Solid when blue is inactive","Red Cycle Block",{80,SCREEN_H-24,8,8},{0}},
    [7]  = {"123","123",{92,SCREEN_H-24,8,8},{0}},
    [8]  = {"Solid when red is inactive","Blue Cycle Block",{104,SCREEN_H-24,8,8},{0}},
    [9]  = {"123","123",{116,SCREEN_H-24,8,8},{0}},
    [10] = {"Disappears and plays sound when collected","Coin",{128,SCREEN_H-24,8,8},{0}},
    [11] = {"Reverses at walls, does not avoid pits","Hedgehog (Enemy)",{140,SCREEN_H-24,8,8},{0}},
    [12] = {"Moves right, reverses direction when it hits a solid block","Moving Pad",{152,SCREEN_H-24,8,8},{0}},
    [13] = {"Grass deco","Grass",{164,SCREEN_H-24,8,8},{0}},
    [14] = {"Cloud deco","Cloud",{176,SCREEN_H-24,16,8},{0}},
    [15] = {"Flower deco","Flowers",{200,SCREEN_H-24,8,8},{0}},
    [16] = {"Used to unlock doors", "Key", {212, SCREEN_H-24, 8, 8}, {0}},
    [17] = {"Needs a key to pass through", "Door", {224, SCREEN_H-24, 8, 8}, {0}},
    [18] = {"Toggle rectangle fill (R)", "Rectangle Fill", {SCREEN_W-24 - 8, SCREEN_H-24, 8, 8}, {0}},
    [19] = {"Toggle camera pan (SHIFT + Left Click or MMB)", "Pan Tool", {SCREEN_W-12, SCREEN_H-24, 8, 8}, {0}},
};

bool mouseOverUi() {
    return (virtualMouse.y >= SCREEN_H - 32) || (virtualMouse.x >= SCREEN_W - 40 && virtualMouse.y <= 40);
}

bool drawbuttonTopRight(Texture2D sprite){
    if (sprite.id == 0) return false;
    float x = (float)(SCREEN_W - sprite.width - 5);
    float y = (float)5;
    Rectangle spriteRec = { x, y, (float)sprite.width, (float)sprite.height };
    bool hovered = CheckCollisionPointRec(virtualMouse, spriteRec);
    DrawTexture(sprite, (int)x, (int)y, hovered ? Fade(WHITE, 0.6f) : WHITE);
    return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

void cameraInput() {
    if ((IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)) || isPanning && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
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

    DrawRectangle(0, SCREEN_H - 32, SCREEN_W, 32, GRAY);

    const char* hoveredName = NULL;
    const char* hoveredDesc = NULL;
    if (tooltips[18].sprite.id == 0) tooltips[18].sprite = LoadTexture("sprites/menu/rectangle.png");
    if (tooltips[19].sprite.id == 0) tooltips[19].sprite = LoadTexture("sprites/menu/pan.png");
    for (int i = 0; i < (SHORTCUTS + TOTAL_BLOCKS); i++) {
        if (i < TOTAL_BLOCKS && tooltips[i].sprite.id == 0) {
            tooltips[i].sprite = blocks[i].sprite;
        }

        Color tint = WHITE;
        if (i == 18 && rectangle) tint = GRAY;
        if (i == 19 && isPanning) tint = GRAY; 

        if (tooltips[i].sprite.id != 0 ) {
            DrawTexture(tooltips[i].sprite, tooltips[i].bounds.x, tooltips[i].bounds.y, tint);
        } else {
            DrawRectangleRec(tooltips[i].bounds, Fade(tint, 0.5f));
        }

        if (i < TOTAL_BLOCKS) {
            if (!blocks[i].showInEdtior) continue;
            if (currentTile == blocks[i].blockID)
                DrawRectangleLinesEx((Rectangle) { tooltips[i].bounds.x - 1, tooltips[i].bounds.y - 1, 10, 10 }, 1, WHITE);
            if (i == 0) DrawTexture(trash, tooltips[i].bounds.x, tooltips[i].bounds.y, WHITE);
        }

        if (CheckCollisionPointRec(virtualMouse, tooltips[i].bounds)) {
            DrawRectangleLinesEx(tooltips[i].bounds, 1, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (i < TOTAL_BLOCKS) {
                    currentTile = blocks[i].blockID;
                } else if (i == 18) {
                    rectangle = !rectangle;
                } else if (i == 19) {
                    isPanning = !isPanning;
                }
            }
            hoveredName = tooltips[i].shortcut;
            hoveredDesc = tooltips[i].description;
        }
    }

    if (hoveredName) {
        int nameWidth  = MeasureText(hoveredName, 10);
        int descWidth  = MeasureText(hoveredDesc, 10);
        int boxWidth   = (nameWidth > descWidth ? nameWidth : descWidth) + 6;
        int boxHeight  = (12 * 2) + 4;
        int boxY       = SCREEN_H - 32 - boxHeight - 2;
        DrawRectangle(2, boxY, boxWidth, boxHeight, Fade(BLACK, 0.75f));
        DrawText(hoveredName, 2 + 3, boxY + 2, 10, WHITE);
        DrawText(hoveredDesc, 2 + 3, boxY + 2 + 12, 10, LIGHTGRAY);
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

    bool isUsingPan = isPanning && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (mapX >= 0 && mapX < WORLD_W && mapY >= 0 && mapY < WORLD_H && !mouseOverUi() && !isUsingPan) {
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
        DrawTexture(blocks[currentTile].sprite,mapX * TILE_SIZE, mapY * TILE_SIZE,Fade(WHITE,0.2));
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
    if (warningFrames > 0) {
        DrawTexture(flagWarning, SCREEN_W - flagWarning.width, 0, WHITE);
        warningFrames--;
    }

    return editing;
}