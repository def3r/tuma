#include "types.h"
#include "utils.h"
#include "vectors.h"

#include <stdio.h>
#include <string.h>

char *parseMacInfo(FILE *f, turingMachine **tm) {
  *tm = makeTuringMachine();
  if (tm == NULL) {
    perror("Unable to allocate truing machine\n");
    exit(EXIT_FAILURE);
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t n = 0;

  char *linestart = NULL;
  while ((n = getline(&line, &len, f)) != -1) {
    len = n;
    linestart = line;
    // line = trim(line, &len);
    coalesceWhitespace(line, &len);
    if (len == 0)
      continue;
    else if (line[0] == '[')
      break;
    else if (line[0] == '#')
      continue;

    char *key, *val;
    ExtractKeyVal(line, key, val);

    if (strcmp(key, "blanksymbol") == 0) {
      ExtractStr(val);
      if (strlen(val) > 0)
        (*tm)->blankSymbol = val[0];

    } else if (strcmp(key, "numofstates") == 0) {
      (*tm)->numStates = atoi(val);

    } else if (strcmp(key, "inputsymbols") == 0) {
      CheckArray(val, key);
      charVector *v;
      ExtractCharArr(v, val);
      (*tm)->inputSymbols = v;

    } else if (strcmp(key, "tapesymbols") == 0) {
      CheckArray(val, key);
      charVector *v;
      ExtractCharArr(v, val);
      (*tm)->tapeSymbols = v;

    } else if (strcmp(key, "finalstates") == 0) {
      CheckArray(val, key);
      intVector *v;
      ExtractIntArr(v, val);
      (*tm)->finalStates = v;

    } else if (strcmp(key, "initialstate") == 0) {
      (*tm)->initState = atoi(val);

    } else if (strcmp(key, "tape") == 0) {
      ExtractStr(val);
      if (strlen(val) > 0) {
        (*tm)->tape = malloc(strlen(val));
        memcpy((*tm)->tape, val, strlen(val));
      }

    } else {
      printf("Unhandled Key val pair [%s = %s]\n", key, val);
    }

    free(linestart);
    line = NULL;
    len = 0;
  }

  return line;
}

char *parseTransition(FILE *f, turingMachine **tm) {
  transition t = {.cur = UINT16_MAX,
                  .dir = SAME,
                  .head = '\0',
                  .next = UINT16_MAX,
                  .write = '\0',
                  .halt = false};

  char *line = NULL;
  size_t len = 0;
  ssize_t n = 0;

  char *linestart = NULL;
  while ((n = getline(&line, &len, f)) != -1) {
    len = n;
    linestart = line;
    coalesceWhitespace(line, &len);
    if (len == 0)
      continue;
    else if (line[0] == '[')
      break;
    else if (line[0] == '#')
      continue;

    char *key, *val;
    ExtractKeyVal(line, key, val);

    if (strcmp(key, "cur") == 0) {
      t.cur = atoi(val);

    } else if (strcmp(key, "head") == 0) {
      ExtractStr(val);
      if (strlen(val) > 0) {
        t.head = val[0];
      }

    } else if (strcmp(key, "next") == 0) {
      t.next = atoi(val);

    } else if (strcmp(key, "dir") == 0) {
      ExtractStr(val);
      t.dir = stodir(val);

    } else if (strcmp(key, "write") == 0) {
      ExtractStr(val);
      if (strlen(val) > 0) {
        t.write = val[0];
      }

    } else if (strcmp(key, "halt") == 0) {
      t.halt = strcmp(val, "true") == 0;

    } else {
      printf("Unhandled Key: %s|\n", line);
    }

    free(linestart);
    line = NULL;
    len = 0;
  }

  if (t.cur == UINT16_MAX) {
    fprintf(stderr, "Expected a cur state for transition\n");
    exit(EXIT_FAILURE);
  }

  VectorPush((*tm)->transitions, t);
  return line;
}

void parseLine(FILE *f, char *line, size_t len, turingMachine **tm) {
  if (len == 0)
    return;
  if (line[0] == '#')
    return;

  if (strcmp(line, "[turingmachine]") == 0) {
    line = parseMacInfo(f, tm);
    parseLine(f, line, strlen(line), tm);

  } else if (strcmp(line, "[[transition]]") == 0) {
    if (tm == NULL) {
      fprintf(stderr, "%s\n", line);
      fprintf(stderr, "Transition for unknown turing machine");
      exit(EXIT_FAILURE);
    }

    line = parseTransition(f, tm);
    parseLine(f, line, strlen(line), tm);
  }
}

void parseTOML(const char *fname, turingMachine **tm) {
  FILE *f = fopen(fname, "r");
  if (!f) {
    perror("Unable to open file");
    exit(EXIT_FAILURE);
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t n = 0;

  char *linestart = NULL;
  while ((n = getline(&line, &len, f)) != -1) {
    len = n;
    linestart = line;
    coalesceWhitespace(line, &len);
    parseLine(f, line, len, tm);

    free(linestart);
    line = NULL;
    len = 0;
  }

  return;
}
