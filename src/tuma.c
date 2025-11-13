#include "raylib.h"
#include "toml.h"
#include "types.h"
#include "uiutils.h"
#include "utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main() {
  turingMachine *tm = NULL;
  parseTOML("tmadd.toml", &tm);
  padTuringMachineTape(tm);
  initTuringMachineState(tm);

  uiCtx ui = {0};
  initUICtx(&ui, tm);

  printf("\n");
#ifdef DEBUG
  debug(tm);
#endif

  InitWindow(ui.w, ui.h, "tuma");
  SetTargetFPS(60);

  bool inputMode = false;
  char tapeBuffer[256] = {0};
  int tapeLen = 0;
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

    if (run)
      DrawText("Runnin >>", 30, 30, 20, GREEN);
    ui.x = 30, ui.y = 50;

    DrawTransitionGraph(&ui, tm);
    DrawTape(&ui, tm);
    DrawInfo(&ui, tm);

    DrawRectangleLinesEx(ui.cur, 4, WHITE);

    // if (inputMode) {
    //   DrawText("Enter new tape:", 100, 100, 24, WHITE);
    //   DrawRectangle(100, 140, 400, 40, GRAY);
    //   DrawText(tapeBuffer, 110, 150, 24, BLACK);
    //
    //   int key = GetCharPressed();
    //     if (IsKeyPressed(KEY_BACKSPACE) && tapeLen > 0) {
    //       tapeBuffer[--tapeLen] = '\0';
    //     } else if (key >= 32 && key <= 126 &&
    //                tapeLen < sizeof(tapeBuffer) - 1) {
    //       tapeBuffer[tapeLen++] = (char)key;
    //       tapeBuffer[tapeLen] = '\0';
    //     }
    //     key = GetCharPressed();
    //   }
    //
    //   if (IsKeyPressed(KEY_ENTER)) {
    //     free(tm->tape);
    //     tm->rawTape = NULL;
    //     tm->tape = calloc(tapeLen + 1, 1);
    //     memcpy(tm->tape, tapeBuffer, tapeLen);
    //     padTuringMachineTape(tm);
    //     initTuringMachineState(tm);
    //     inputMode = false;
    //   }
    //
    //   if (IsKeyPressed(KEY_ESCAPE)) {
    //     inputMode = false;
    //   }
    EndDrawing();
    // clang-format on

    // Reset Tape
    if (IsKeyPressed(KEY_R)) {
      memcpy(tm->tape, ui.initTape, strlen(ui.initTape));
      initTuringMachineState(tm);
      ui.offsetX = 0;
      ui.dX = 0;
      run = false;
      noop = 30;

      // Run
    } else if (IsKeyPressed(KEY_ENTER) && run == false && inputMode == false) {
      run = true;
      noop = 30;

      // Pause
    } else if (IsKeyPressed(KEY_P)) {
      run = false;
      noop = 30;

      // Quit
    } else if (IsKeyPressed(KEY_Q)) {
      break;

      // Load New File
    } else if (IsKeyPressed(KEY_O)) {

      // New Tape
    } else if (IsKeyPressed(KEY_T)) {
      inputMode = true;

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
  printf("Output: %.*s\n", (int)tm->rawTapeLen, tm->rawTape);

  free(ui.initTape);

  freeTuringMachine(tm);
}
