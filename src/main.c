#include "raylib.h"
#include "toml.h"
#include "tuma.h"
#include "uiutils.h"
#include "utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#define MAX_FILEPATH_RECORDED 4096
#define MAX_FILEPATH_SIZE 2048

void LoadTuma(char *filePath, uiCtx *ui, turingMachine **tm) {
  if (strlen(filePath) == 0) {
    *tm = makeTuringMachine();
    padTuringMachineTape(*tm);
    initTuringMachineState(*tm);
    initUICtx(ui, *tm);
    return;
  }

  void *ptr = *tm;
  parseTOML(filePath, tm);
  if (*tm == ptr)
    return;
  padTuringMachineTape(*tm);
  initTuringMachineState(*tm);
  initUICtx(ui, *tm);

  printf("\n");
#ifdef DEBUG
  debug(*tm);
#endif
}

int main() {
  turingMachine *tm = NULL;
  uiCtx ui = {0};
  LoadTuma("", &ui, &tm);

  InitWindow(ui.w, ui.h, "tuma");
  SetTargetFPS(60);

  int filePathCounter = 0;
  char *filePaths[MAX_FILEPATH_RECORDED] = {0};
  for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
    filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
  }

  bool inputMode = false;
  char tapeBuffer[256] = {0};
  int tapeLen = 0;
  bool run = false;
  int noop = 30;

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();

      for (int i = 0, offset = filePathCounter; i < (int)droppedFiles.count;
           i++) {
        if (filePathCounter < (MAX_FILEPATH_RECORDED - 1)) {
          TextCopy(filePaths[offset + i], droppedFiles.paths[i]);
          filePathCounter++;
        }
      }
      UnloadDroppedFiles(droppedFiles);

      printf("Last File: %s\n", filePaths[filePathCounter - 1]);
      LoadTuma(filePaths[filePathCounter - 1], &ui, &tm);
      tapeLen = 0;
      run = false;
      noop = 30;
    }

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
    EndDrawing();
    // clang-format on

    // Reload config
    if (filePathCounter > 0 && IsKeyDown(KEY_LEFT_CONTROL) &&
        IsKeyPressed(KEY_R)) {
      LoadTuma(filePaths[filePathCounter - 1], &ui, &tm);

      // Reset Tape
    } else if (IsKeyPressed(KEY_R)) {
      memcpy(tm->tape, ui.initTape, strlen(ui.initTape));
      initTuringMachineState(tm);
      ui.offsetX = 0;
      ui.dX = 0;
      run = false;
      noop = 30;

      // Run
    } else if (filePathCounter > 0 && IsKeyPressed(KEY_ENTER) && run == false &&
               inputMode == false) {
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
      // inputMode = true;

      // Execute
    } else if (filePathCounter > 0 && IsKeyPressed(KEY_E)) {
      int headIndexInit = (tm->state.head - tm->tape);
      execTuringMachine(tm);
      int headIndex = (tm->state.head - tm->tape);
      ui.offsetX += (headIndex - headIndexInit) * ui.charWidth;

      // Next Step
    } else if (filePathCounter > 0 && (IsKeyPressed(KEY_SPACE) || run)) {
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

  for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
    RL_FREE(filePaths[i]);
  }

  CloseWindow();

  if (filePathCounter > 0)
    execTuringMachine(tm);
  if (tm->rawTapeLen > 0)
    printf("Output: %.*s\n", (int)tm->rawTapeLen, tm->rawTape);

  free(ui.initTape);

  freeTuringMachine(tm);
}
