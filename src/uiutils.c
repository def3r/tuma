#include "uiutils.h"
#include "raylib.h"
#include "raymath.h"
#include "tuma.h"
#include "utils.h"
#include "vectors.h"

#include <math.h>
#include <stdint.h>
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

  int len = (tm->tape == NULL) ? 0 : strlen(tm->tape);
  ctx->initTape = malloc(len + 1);
  memcpy(ctx->initTape, tm->tape, len + 1);

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
  Color c;
  sprintf(buf, "%d", tm->state.curState);
  DrawText("State", ctx->x, ctx->y, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y, 26, GRAY);

  sprintf(buf, "%ld", (tm->state.head - tm->tape - 10));
  DrawText("Head index", ctx->x, ctx->y + 30, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 30, 26, GRAY);

  sprintf(buf, "%u", tm->state.steps);
  DrawText("Steps", ctx->x, ctx->y + 60, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 60, 26, GRAY);

  sprintf(buf, "%s",
          (tm->state.halt || tm->rawTapeLen == 0) ? "Halt" : "Running");
  DrawText("Status", ctx->x, ctx->y + 90, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 90, 26, GRAY);

  bool isFinalState = false;
  VectorFind(tm->finalStates, tm->state.curState, isFinalState);
  sprintf(buf, "%s", (isFinalState) ? "True" : "False");
  c = isFinalState ? GREEN : (Color){155, 55, 55, 255};
  DrawText("Accept", ctx->x, ctx->y + 120, 26, GRAY);
  DrawText(buf, ctx->x + 180, ctx->y + 120, 26, c);

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
    Color color = active ? SKYBLUE : LIGHTGRAY;

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

void DrawTransitionGraph(uiCtx *ctx, turingMachine *tm) {
  float arrowOffset[MAXSTATES][MAXSTATES][2] = {0};
  memset(arrowOffset, 0,
         sizeof(arrowOffset[0][0][0]) * MAXSTATES * MAXSTATES * 2);
  int minOffset = 0;
  int offsetInc = 20;

  Vector2 *nodes = malloc(sizeof(Vector2) * tm->numStates);
  int padding = 80;
  int stateRadius = 25;
  int totalWidth = ctx->w - 2 * padding;
  float step = (float)totalWidth / (tm->numStates);

  for (int i = 0; i < tm->numStates; i++) {
    nodes[i].x = ctx->x + padding + (int)(i * step);
    nodes[i].y = ctx->y + (int)(ctx->h / 6);
  }

  float amplitude = 40.0f; // vertical offset (height of the arc)
  for (size_t i = 0; i < tm->transitions->len; i++) {
    transition *t = &tm->transitions->arr[i];
    if (t->cur >= tm->numStates || t->next == UINT16_MAX ||
        t->next >= tm->numStates)
      continue;

    Vector2 a = {nodes[t->cur].x, nodes[t->cur].y};
    Vector2 b = {nodes[t->next].x, nodes[t->next].y};

    int sign = (i % 2 == 0) ? 1 : -1;
    int idx = (sign > 0);

    Vector2 mid = {(a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f + sign * amplitude};

    bool active = (t->cur == tm->state.curState) &&
                  ((t->head == *tm->state.head) || t->head == '\0');
    Color lineColor = active ? SKYBLUE : GRAY;

    if (t->cur == t->next) {
      Vector2 c = {nodes[t->cur].x, nodes[t->cur].y};
      float offset = 40.0f;
      float side = 25.0f;

      int y = sign * stateRadius;
      int aOff = 0;

      if (sign > 0)
        y = stateRadius;

      if (minOffset > arrowOffset[t->cur][t->cur][idx]) {
        aOff += minOffset;
        arrowOffset[t->cur][t->cur][idx] = minOffset;
      } else {
        aOff += arrowOffset[t->cur][t->cur][idx];
      }
      minOffset = fmax(minOffset, arrowOffset[t->cur][t->next][idx]);
      arrowOffset[t->cur][t->cur][idx] += offsetInc++;

      Vector2 p1 = (Vector2){c.x - (int)(stateRadius / 2), c.y + y};
      Vector2 p2 = (Vector2){c.x - (int)(stateRadius / 2),
                             p1.y + sign * (offset + aOff)};
      Vector2 p3 = (Vector2){c.x - (int)(stateRadius / 2) + side, p2.y};
      Vector2 p4 = (Vector2){c.x - (int)(stateRadius / 2) + side, c.y + y};

      DrawLineEx(p1, p2, 2, lineColor);
      DrawLineEx(p2, p3, 2, lineColor);
      DrawLineEx(p3, p4, 2, lineColor);

      Vector2 dir = Vector2Normalize((Vector2){p4.x - p3.x, p4.y - p3.y});
      Vector2 sideVec = {-dir.y, dir.x};
      float head_len = 10.0f, head_wid = 5.0f;

      Vector2 tip = (Vector2){p4.x - dir.x * 2, p4.y - dir.y * 2};
      Vector2 left = (Vector2){tip.x - dir.x * head_len + sideVec.x * head_wid,
                               tip.y - dir.y * head_len + sideVec.y * head_wid};
      Vector2 right =
          (Vector2){tip.x - dir.x * head_len - sideVec.x * head_wid,
                    tip.y - dir.y * head_len + sideVec.y * head_wid};

      DrawLineEx(tip, left, 2, lineColor);
      DrawLineEx(tip, right, 2, lineColor);

      continue;
    }

    int mxOff = fmax(arrowOffset[t->next][t->cur][idx],
                     arrowOffset[t->cur][t->next][idx]);
    if (minOffset > mxOff) {
      mid.y += sign * minOffset;
      arrowOffset[t->cur][t->next][idx] = minOffset + (int)(offsetInc / 2);
      arrowOffset[t->next][t->cur][idx] = minOffset + (int)(offsetInc / 2);
    } else {
      mid.y += fmax(arrowOffset[t->next][t->cur][idx],
                    arrowOffset[t->cur][t->next][idx]);
      arrowOffset[t->cur][t->next][idx] += offsetInc++;
      arrowOffset[t->next][t->cur][idx] += offsetInc++;
    }
    minOffset = fmax(minOffset, arrowOffset[t->cur][t->next][idx]);

    // if (sign < 0) {
    //   if (minOffset > arrowOffset[t->cur][t->next][1]) {
    //     mid.y -= minOffset;
    //     arrowOffset[t->cur][t->next][1] = minOffset;
    //   } else {
    //     mid.y -= arrowOffset[t->cur][t->next][1];
    //     arrowOffset[t->cur][t->next][1] += 20;
    //   }
    //   minOffset = fmax(minOffset, arrowOffset[t->cur][t->next][1]);
    // } else {
    //   if (minOffset > arrowOffset[t->cur][t->next][0]) {
    //     mid.y += minOffset;
    //     arrowOffset[t->cur][t->next][1] = minOffset;
    //   } else {
    //     mid.y += arrowOffset[t->cur][t->next][0];
    //     arrowOffset[t->cur][t->next][0] += 20;
    //   }
    //   minOffset = fmax(minOffset, arrowOffset[t->cur][t->next][0]);
    // }

    DrawLineEx(a, mid, 2, lineColor);
    DrawLineEx(mid, b, 2, lineColor);

    Vector2 dir = Vector2Normalize((Vector2){b.x - mid.x, b.y - mid.y});
    Vector2 tip = {b.x - dir.x * (stateRadius + 8),
                   b.y - dir.y * (stateRadius + 8)};

    // perpendicular vector for the arrowhead sides
    Vector2 side = {-dir.y, dir.x};
    float head_len = 12.0f; // length of each small arrow line
    float head_ang = 0.5f;  // controls spread — 0.5 is ~30°
    Vector2 left_tip = {tip.x - dir.x * head_len + side.x * head_len * head_ang,
                        tip.y - dir.y * head_len +
                            side.y * head_len * head_ang};
    Vector2 right_tip = {
        tip.x - dir.x * head_len - side.x * head_len * head_ang,
        tip.y - dir.y * head_len - side.y * head_len * head_ang};

    DrawLineEx(tip, left_tip, 2, lineColor);
    DrawLineEx(tip, right_tip, 2, lineColor);
  }

  for (int i = 0; i < tm->numStates; i++) {
    Color color = (tm->state.curState == i) ? TABOUTLINE : LIGHTGRAY;
    DrawCircle(nodes[i].x, nodes[i].y, stateRadius, color);

    char buf[16];
    sprintf(buf, "%d", i);
    int tw = MeasureText(buf, 20);
    DrawText(buf, nodes[i].x - (int)(tw / 2), nodes[i].y - 10, 20, BLACK);
  }

  free(nodes);
}
