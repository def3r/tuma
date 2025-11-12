#include "types.h"
#include "utils.h"
#include "vectors.h"

#include <stdio.h>
#include <string.h>

turingMachine *makeTuringMachine() {
  turingMachine *tm = malloc(sizeof(turingMachine));
  if (tm == NULL)
    return NULL;
  memset(tm, 0, sizeof(turingMachine));
  tm->blankSymbol = 'B';
  tm->tape = "";
  MakeVector(transitionVector, tm->transitions);
  return tm;
}

void freeTuringMachine(turingMachine *tm) {
  tm->state.head = NULL;
  Free(tm->tape);
  FreeVector(tm->inputSymbols);
  FreeVector(tm->tapeSymbols);
  FreeVector(tm->finalStates);
  FreeVector(tm->transitions);
  Free(tm);
}

void initTuringMachineState(turingMachine *tm) {
  tm->state.head = tm->tape;
  tm->state.curState = tm->initState;
  tm->state.dir = RIGHT;
}

void transition_func(turingMachine *tm) {
  // transition for cur, head
  const transition *t = NULL;
  int i = 0;
  for (i = 0; i < tm->transitions->len; i++) {
    t = &tm->transitions->arr[i];
    if (t->cur == tm->state.curState &&
        (t->head == *tm->state.head || t->head == '\0')) {
      break;
    }
  }

  if (i != tm->transitions->len) {
    if (t->halt) {
      tm->state.halt = true;
    }
    if (t->write != '\0') {
      *tm->state.head = t->write;
    }
    if (t->dir != SAME) {
      tm->state.dir = t->dir;
    }
    tm->state.curState = t->next;
  }
  tm->state.head += tm->state.dir;
}

void exec(turingMachine *tm) {
  initTuringMachineState(tm);
  bool exit = false;
  while (!exit && !tm->state.halt) {
    printf("Current State: %d, head: %c, tape: %s\n", tm->state.curState,
           *tm->state.head, tm->tape);
    transition_func(tm);
    VectorFind(tm->finalStates, tm->state.curState, exit);
  }
}
