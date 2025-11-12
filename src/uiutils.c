#include "uiutils.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initUICtx(uiCtx *ctx, turingMachine *tm) {
  ctx->h = 800, ctx->w = 1300;
  ctx->charWidth = 100, ctx->charHeight = 90;
  ctx->fontSize = 60;

  ctx->x = 0, ctx->y = 0;

  ctx->curX = ctx->w / 2 - (int)(ctx->charWidth / 2);
  ctx->curY = ctx->h / 2 - (int)(ctx->charHeight / 2);
  ctx->curPadding = 10;

  ctx->cur = (Rectangle){
      .x = ctx->curX - ctx->curPadding,
      .y = ctx->curY - ctx->curPadding,
      .width = ctx->charWidth + 2 * ctx->curPadding,
      .height = ctx->charHeight + 2 * ctx->curPadding,
  };

  ctx->initTape = malloc(strlen(tm->tape) + 1);
  memcpy(ctx->initTape, tm->tape, strlen(tm->tape) + 1);

  ctx->offsetX = 0;
  ctx->dX = 0;
}

void DrawTape(uiCtx *ctx, turingMachine *tm) {
  char *tape = tm->tape;
  ctx->x = ctx->w / 2 - (int)(ctx->charWidth / 2) - (10 * ctx->charWidth) -
           ctx->offsetX,
  ctx->y = ctx->h / 2 - (int)(ctx->charHeight / 2);
  while (*tape != '\0') {
    char c[2];
    c[0] = *tape, c[1] = '\0';
    DrawCharRectangle(ctx, c);
    tape++;
  }
}

void DrawCharRectangle(uiCtx *ctx, char *s) {
  Rectangle r = {
      .x = ctx->x,
      .y = ctx->y,
      .width = ctx->charWidth,
      .height = ctx->charHeight,
  };

  DrawRectangleRec(r, TABDIM);
  DrawRectangleLinesEx(r, 2, TABOUTLINE);

  int textWidth = MeasureText(s, ctx->fontSize);
  int textHeight = ctx->fontSize;
  DrawText(s, ctx->x + (ctx->charWidth - textWidth) / 2,
           ctx->y + (ctx->charHeight - textHeight) / 2, ctx->fontSize, BGCOLOR);

  ctx->x += ctx->charWidth;
}

void DrawInfo(uiCtx *ctx, turingMachine *tm) {
  ctx->y += 1.5 * ctx->charHeight;
  ctx->x = 30;
  char buf[BUFSIZ];
  sprintf(buf, "%d", tm->state.curState);
  DrawText("State", ctx->x, ctx->y, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y, 26, GRAY);

  sprintf(buf, "%ld", (tm->state.head - tm->tape - 10));
  DrawText("Head index", ctx->x, ctx->y + 30, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 30, 26, GRAY);

  sprintf(buf, "%u", tm->state.steps);
  DrawText("Steps", ctx->x, ctx->y + 60, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 60, 26, GRAY);

  sprintf(buf, "Transitions for State %d", tm->state.curState);
  DrawText(buf, ctx->x + ctx->w / 2, ctx->y, 26, LIGHTGRAY);
  ctx->x += ctx->w / 2;
  ctx->y += 30;

  int col_x[] = {
      ctx->x + 10,  // Read
      ctx->x + 100, // Write
      ctx->x + 200, // Next
      ctx->x + 300, // Move
      ctx->x + 400  // Halt
  };

  int y = ctx->y + 30;
  int lineHeight = 30;

  const char *headers[] = {"Read", "Write", "Next", "Move", "Halt"};
  for (int i = 0; i < 5; i++) {
    DrawText(headers[i], col_x[i], y, 20, GRAY);
  }
  y += lineHeight + 4;

  DrawLine(ctx->x, y - 4, ctx->x + 450, y - 4, DARKGRAY);

  for (size_t i = 0; i < tm->transitions->len; i++) {
    transition *t = &tm->transitions->arr[i];
    if (t->cur != tm->state.curState)
      continue;

    bool active = (t->head == *tm->state.head || t->head == '\0');
    Color color = active ? TABOUTLINE : LIGHTGRAY;

    char buf2[64];
    sprintf(buf2, "%c", t->head);
    DrawText(buf2, col_x[0], y, 20, color);

    sprintf(buf2, "%c", t->write);
    DrawText(buf2, col_x[1], y, 20, color);

    if (t->next == UINT16_MAX)
      memset(buf, 0, BUFSIZ);
    else
      sprintf(buf2, "%d", t->next);
    DrawText(buf2, col_x[2], y, 20, color);

    sprintf(buf2, "%c", (t->dir == LEFT ? 'L' : (t->dir == RIGHT ? 'R' : 'N')));
    DrawText(buf2, col_x[3], y, 20, color);

    DrawText(t->halt ? "yes" : "no", col_x[4], y, 20, color);

    y += lineHeight;
  }
}
