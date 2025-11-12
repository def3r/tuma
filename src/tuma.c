#include "raylib.h"
#include "toml.h"
#include "types.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define BGCOLOR (Color){23, 23, 23, 255}
#define FGCOLOR_TAB (Color){184, 187, 241, 255}
#define FGCOLOR_TABDIM (Color){184, 187, 241, 85}

#include <stdio.h>

int main() {
  turingMachine *tm = NULL;
  parseTOML("tmadd.toml", &tm);
  initTuringMachineState(tm);
  printf("\n");
#ifdef DEBUG
  debug(tm);
#endif

  const int h = 800, w = 1300;
  InitWindow(w, h, "tuma");
  SetTargetFPS(60);

  const int charWidth = 100, charHeight = 90;
  const int fontSize = 60;

  const int curX = w / 2 - (int)(charWidth / 2);
  const int curY = h / 2 - (int)(charHeight / 2);
  const int curPadding = 10;

  Rectangle cur = {
      .x = curX - curPadding,
      .y = curY - curPadding,
      .width = charWidth + 2 * curPadding,
      .height = charHeight + 2 * curPadding,
  };

  char *initTape = malloc(strlen(tm->tape) + 1);
  memcpy(initTape, tm->tape, strlen(tm->tape) + 1);

  int offsetX = 0;
  int dX = 0;

  while (!WindowShouldClose()) {
    if (dX > 0) {
      offsetX += 5;
      dX -= 5;
    } else if (dX < 0) {
      offsetX -= 5;
      dX += 5;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);

    // DrawLine(0, h / 2 - charHeight / 2, w, h / 2 - charHeight / 2, GRAY);

    char *text = tm->tape;

    int x = 0 - offsetX, y = h / 2 - (int)(charHeight / 2);
    for (int i = 0; i < 6; i++) {
      Rectangle r = {
          .x = x,
          .y = y,
          .width = charWidth,
          .height = charHeight,
      };

      DrawRectangleRec(r, FGCOLOR_TABDIM);
      DrawRectangleLinesEx(r, 2, FGCOLOR_TAB);

      int textWidth = MeasureText(&tm->blankSymbol, fontSize);
      int textHeight = fontSize;
      DrawText(&tm->blankSymbol, x + (100 - textWidth) / 2,
               y + (90 - textHeight) / 2, fontSize, BGCOLOR);
      x += charWidth;
    }

    x = w / 2 - (int)(charWidth / 2) - offsetX,
    y = h / 2 - (int)(charHeight / 2);
    while (*text != '\0') {
      Rectangle r = {
          .x = x,
          .y = y,
          .width = charWidth,
          .height = charHeight,
      };

      DrawRectangleRec(r, FGCOLOR_TABDIM);
      DrawRectangleLinesEx(r, 2, FGCOLOR_TAB);

      char c[2];
      c[0] = *text;
      c[1] = '\0';
      int textWidth = MeasureText(c, fontSize);
      int textHeight = fontSize;
      DrawText(c, x + (100 - textWidth) / 2, y + (90 - textHeight) / 2,
               fontSize, BGCOLOR);
      x += charWidth;
      text++;
    }

    for (int i = 0; i < 14; i++) {
      Rectangle r = {
          .x = x,
          .y = y,
          .width = charWidth,
          .height = charHeight,
      };

      DrawRectangleRec(r, FGCOLOR_TABDIM);
      DrawRectangleLinesEx(r, 2, FGCOLOR_TAB);

      int textWidth = MeasureText(&tm->blankSymbol, fontSize);
      int textHeight = fontSize;
      DrawText(&tm->blankSymbol, x + (100 - textWidth) / 2,
               y + (90 - textHeight) / 2, fontSize, BGCOLOR);
      x += charWidth;
    }

    // DrawLine(0, h / 2 + charHeight / 2, w, h / 2 + charHeight / 2, GRAY);

    DrawRectangleLinesEx(cur, 4, WHITE);

    EndDrawing();

    if (IsKeyPressed(KEY_SPACE)) {
      nextTuringMachine(tm);
      if (tm->state.halt)
        continue;
      if (tm->state.dir == RIGHT) {
        dX += charWidth;
      } else if (tm->state.dir == LEFT) {
        dX += -charWidth;
      }
    } else if (IsKeyPressed(KEY_R)) {
      memcpy(tm->tape, initTape, strlen(initTape));
      initTuringMachineState(tm);
      offsetX = 0;
      dX = 0;
    } else if (IsKeyPressed(KEY_Q)) {
      break;
    }
  }
  CloseWindow();

  // execTuringMachine(tm);
  printf("Output: %s\n", tm->tape);

  free(initTape);

  freeTuringMachine(tm);
}
