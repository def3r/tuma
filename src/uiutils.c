#include "uiutils.h"
#include "types.h"

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

void DrawPrefixTape(uiCtx *ctx, turingMachine *tm) {
  ctx->x = 0 - ctx->offsetX, ctx->y = ctx->h / 2 - (int)(ctx->charHeight / 2);
  for (int i = 0; i < 6; i++) {
    DrawCharRectangle(ctx, &tm->blankSymbol);
  }
}

void DrawTape(uiCtx *ctx, turingMachine *tm) {
  char *tape = tm->tape;
  ctx->x = ctx->w / 2 - (int)(ctx->charWidth / 2) - ctx->offsetX,
  ctx->y = ctx->h / 2 - (int)(ctx->charHeight / 2);
  while (*tape != '\0') {
    char c[2];
    c[0] = *tape, c[1] = '\0';
    DrawCharRectangle(ctx, c);
    tape++;
  }
}

void DrawSuffixTape(uiCtx *ctx, turingMachine *tm) {
  for (int i = 0; i < 14; i++) {
    Rectangle r = {
        .x = ctx->x,
        .y = ctx->y,
        .width = ctx->charWidth,
        .height = ctx->charHeight,
    };

    DrawRectangleRec(r, TABDIM);
    DrawRectangleLinesEx(r, 2, TABOUTLINE);

    int textWidth = MeasureText(&tm->blankSymbol, ctx->fontSize);
    int textHeight = ctx->fontSize;
    DrawText(&tm->blankSymbol, ctx->x + (100 - textWidth) / 2,
             ctx->y + (90 - textHeight) / 2, ctx->fontSize, BGCOLOR);
    ctx->x += ctx->charWidth;
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
