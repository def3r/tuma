#include "raylib.h"
#include "toml.h"
#include "types.h"
#include "uiutils.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main() {
  turingMachine *tm = NULL;
  parseTOML("tm.toml", &tm);
  initTuringMachineState(tm);

  uiCtx ui = {0};
  initUICtx(&ui, tm);

  printf("\n");
#ifdef DEBUG
  debug(tm);
#endif

  InitWindow(ui.w, ui.h, "tuma");
  SetTargetFPS(60);

  bool run = false;
  int noop = 30;
  while (!WindowShouldClose()) {
    if (ui.dX > 0) {
      ui.offsetX += 5;
      ui.dX -= 5;
    } else if (ui.dX < 0) {
      ui.offsetX -= 5;
      ui.dX += 5;
    }

    if (run && ui.dX == 0) {
      noop -= run;
    }

    // clang-format off
    BeginDrawing();
      ClearBackground(BGCOLOR);

      DrawPrefixTape(&ui, tm);
      DrawTape(&ui, tm);
      DrawSuffixTape(&ui, tm);

      DrawRectangleLinesEx(ui.cur, 4, WHITE);
    EndDrawing();
    // clang-format off

      // Reset Tape
    if (IsKeyPressed(KEY_R)) {
      memcpy(tm->tape, ui.initTape, strlen(ui.initTape));
      initTuringMachineState(tm);
      ui.offsetX = 0;
      ui.dX = 0;
      run = false;
      noop = 30;

      // Run
    } else if (IsKeyPressed(KEY_ENTER) && run == false) {
      run = true;

      // Quit
    } else if (IsKeyPressed(KEY_Q)) {
      break;

      // Next Step
    } else if (IsKeyPressed(KEY_SPACE) || run) {
      if (run && ui.dX != 0)
        continue;
      if (run && noop != 0)
        continue;

      nextTuringMachine(tm);
      if (tm->state.halt) {
        run = false;
        continue;
      }
      if (tm->state.dir == RIGHT) {
        ui.dX += ui.charWidth;
      } else if (tm->state.dir == LEFT) {
        ui.dX += -ui.charWidth;
      }

      noop = 30;
    }

  }
  CloseWindow();

  // execTuringMachine(tm);
  printf("Output: %s\n", tm->tape);

  free(ui.initTape);

  freeTuringMachine(tm);
}
