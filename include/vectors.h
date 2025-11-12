#ifndef VECTORS_H_
#define VECTORS_H_

#include <stdlib.h>

#define MakeVector(vType, v)                                                   \
  do {                                                                         \
    v = malloc(sizeof(vType));                                                 \
    if (v != NULL) {                                                           \
      memset(v, 0, sizeof(vType));                                             \
    }                                                                          \
  } while (0)

#define FreeVector(v)                                                          \
  free(v->arr);                                                                \
  free(v);                                                                     \
  v = NULL

#define VectorPush(v, c)                                                       \
  if (v != NULL) {                                                             \
    if (v->len == v->capacity) {                                               \
      v->capacity = v->capacity == 0 ? 1 : v->capacity;                        \
      void *newArr = malloc(sizeof(*v->arr) * 2 * v->capacity);                \
      memcpy(newArr, v->arr, sizeof(*v->arr) * v->len);                        \
      free(v->arr);                                                            \
      v->arr = newArr;                                                         \
      v->capacity *= 2;                                                        \
    }                                                                          \
    v->arr[v->len++] = c;                                                      \
  }

#define VectorFind(v, c, exists)                                               \
  if (v != NULL) {                                                             \
    exists = false;                                                            \
    for (int i = 0; i < v->len; i++) {                                         \
      if (v->arr[i] == c) {                                                    \
        exists = true;                                                         \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

#endif // !VECTORS_H_
