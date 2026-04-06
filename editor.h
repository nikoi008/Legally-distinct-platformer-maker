#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"
bool mouseOverUi();
void cameraInput();
void tileSelect();
bool editorFrame(Texture2D flagWarning);
void rectangleMode(Vector2 firstTilePos);
void fillRectangle(Vector2 startPoint, Vector2 endPoint, int tileId);

#endif