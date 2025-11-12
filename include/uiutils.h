#ifndef UIUTILS_H_
#define UIUTILS_H_

#include "raylib.h"
#include "types.h"

#define BGCOLOR (Color){23, 23, 23, 255}
#define TABOUTLINE (Color){184, 187, 241, 255}
#define TABDIM (Color){184, 187, 241, 85}

// clang-format off
typedef struct {
  int   h, w;

  int   charWidth, charHeight;
  int   fontSize;

  int   x, y;

  int   curX;
  int   curY;
  int   curPadding;

  int   offsetX;
  int   dX;
  char *initTape;

  Rectangle cur;
} uiCtx;
// clang-format on

void DrawCharRectangle(uiCtx *ctx, char *s);

void initUICtx(uiCtx *ctx, turingMachine *tm);
void DrawPrefixTape(uiCtx *ctx, turingMachine *tm);
void DrawTape(uiCtx *ctx, turingMachine *tm);
void DrawSuffixTape(uiCtx *ctx, turingMachine *tm);

void DrawInfo(uiCtx *ctx, turingMachine *tm);

#endif
