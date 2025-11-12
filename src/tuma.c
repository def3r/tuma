#include "raylib.h"
#include "toml.h"
#include "types.h"
#include "utils.h"

#define DEBUG

#define BGCOLOR (Color){23, 23, 23, 255}

#include <stdio.h>

int main() {
  turingMachine *tm = NULL;
  parseTOML("tmadd.toml", &tm);
  printf("\n");
#ifdef DEBUG
  debug(tm);
#endif

  int h = 800, w = 1300;
  InitWindow(w, h, "tuma");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BGCOLOR);
    EndDrawing();
  }

  exec(tm);
  printf("Output: %s\n", tm->tape);

  freeTuringMachine(tm);
}
