#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum Dir { LEFT = -1, RIGHT = 1, NOMOVE = 0, SAME = 2 };

// clang-format off

typedef struct {
  char   *arr;
  size_t  len;
  size_t  capacity;
} charVector;

typedef struct {
  int    *arr;
  size_t  len;
  size_t  capacity;
} intVector;

typedef struct {
  uint16_t cur, next;
  char     head;
  char     write;
  enum Dir dir;
  bool     halt;
} transition;

typedef struct {
  transition *arr;
  size_t      len;
  size_t      capacity;
} transitionVector;
typedef struct {
  transitionVector *arr;
  size_t            len;
  size_t            capacity;
} transitionVector2;

typedef struct {
  uint32_t  steps;
  uint16_t  curState;
  enum Dir  dir;
  bool      halt;
  char     *head;
} turingMachineState;

typedef struct {
  uint16_t           numStates;
  uint16_t           initState;
  char               blankSymbol;
  char              *tape;
  char              *rawTape;
  size_t             rawTapeLen;
  charVector        *inputSymbols;
  charVector        *tapeSymbols;
  intVector         *finalStates;
  transitionVector  *transitions;
  turingMachineState state;
} turingMachine;

// clang-format on

turingMachine *makeTuringMachine();
void freeTuringMachine(turingMachine *tm);
void initTuringMachineState(turingMachine *tm);
void padTuringMachineTape(turingMachine *tm);
void transition_func(turingMachine *tm);
void execTuringMachine(turingMachine *tm);
void nextTuringMachine(turingMachine *tm);

#endif
