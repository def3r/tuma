#ifndef UTILS_H_
#define UTILS_H_

#include "types.h"

#include <stdbool.h>

#define Free(v)                                                                \
  free(v);                                                                     \
  v = NULL

#define CheckArray(val, key)                                                   \
  unsigned long arrLen = strlen(val);                                          \
  if (arrLen == 0 || val[0] != '[' || val[arrLen - 1] != ']') {                \
    fprintf(stderr, "Expected an array [...] for %s\n", key);                  \
    exit(EXIT_FAILURE);                                                        \
  }

#define ExtractStr(val)                                                        \
  val += val[0] == '\'';                                                       \
  if (val[strlen(val) - 1] == '\'') {                                          \
    val[strlen(val) - 1] = '\0';                                               \
  }

#define ExtractCharArr(v, val)                                                 \
  MakeVector(charVector, v);                                                   \
  char *token;                                                                 \
  token = strtok(val, "[,]");                                                  \
  while (token != NULL) {                                                      \
    ExtractStr(token);                                                         \
    if (strlen(token) > 0)                                                     \
      VectorPush(v, token[0]);                                                 \
    token = strtok(NULL, "[,]");                                               \
  }

#define ExtractIntArr(v, val)                                                  \
  MakeVector(intVector, v);                                                    \
  char *token;                                                                 \
  token = strtok(val, "[,]");                                                  \
  while (token != NULL) {                                                      \
    VectorPush(v, atoi(token));                                                \
    token = strtok(NULL, "[,]");                                               \
  }

#define ExtractKeyVal(line, var, val)                                          \
  char *c = line;                                                              \
  for (; *c != '\0' && *c != '='; c++)                                         \
    ;                                                                          \
  if (*c != '=') {                                                             \
    fprintf(stderr, "Missing '=' in toml file");                               \
    exit(EXIT_FAILURE);                                                        \
  }                                                                            \
                                                                               \
  *c = '\0';                                                                   \
  var = line;                                                                  \
  val = ++c;

bool isWhitespace(char c);
enum Dir stodir(const char *c);
void coalesceWhitespace(char *line, size_t *len);
void debug(turingMachine *tm);

#endif // !UTILS_H_
