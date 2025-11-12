#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

enum Dir stodir(const char *c) {
  if (strcmp(c, "LEFT") == 0)
    return LEFT;
  if (strcmp(c, "RIGHT") == 0)
    return RIGHT;
  if (strcmp(c, "NOMOVE") == 0)
    return NOMOVE;
  return SAME;
}

void coalesceWhitespace(char *line, size_t *len) {
  if (*len == 0)
    return;

  char buf[*len + 1];
  size_t bufIndex = 0;
  size_t i = 0;
  while (i < *len && line[i] != '\0') {
    if (!isWhitespace(line[i])) {
      if (line[i] == '#' && (i == 0 || line[i - 1] != '\\')) {
        break;
      }
      buf[bufIndex++] = line[i];
    }
    i++;
  }

  *len = bufIndex;
  buf[bufIndex++] = '\0';
  while (bufIndex--) {
    line[bufIndex] = buf[bufIndex];
  }
}

void debug(turingMachine *tm) {
  printf("Loaded data:\nTuring Machine:\n");
  printf("Raw Tape: %.*s\n", (int)tm->rawTapeLen, tm->rawTape);
  printf("Tape: %s\n", tm->tape);
  printf("No of States: %d\n", tm->numStates);
  printf("Blank sym: %c\n", tm->blankSymbol);
  printf("init state: %d\n", tm->initState);
  printf("final states: ");
  for (int i = 0; i < tm->finalStates->len; i++) {
    printf("%d,", tm->finalStates->arr[i]);
  }

  printf("\ninput symbols: ");
  for (int i = 0; i < tm->inputSymbols->len; i++) {
    printf("%c,", tm->inputSymbols->arr[i]);
  }

  printf("\ntape symbols: ");
  for (int i = 0; i < tm->tapeSymbols->len; i++) {
    printf("%c,", tm->tapeSymbols->arr[i]);
  }

  printf("\ntransitions: ");

  if (tm->transitions != NULL) {
    printf("total = %ld\n", tm->transitions->len);
    for (int i = 0; i < tm->transitions->len; i++) {
      printf("  Transition #%d\n", i);
      printf("    cur: %d, next: %d, head: %c, write: %c, dir: %d, halt: %d\n",
             tm->transitions->arr[i].cur, tm->transitions->arr[i].next,
             tm->transitions->arr[i].head, tm->transitions->arr[i].write,
             tm->transitions->arr[i].dir, tm->transitions->arr[i].halt);
    }
  } else {
    printf("No transitinos for the tm\n");
  }
}
