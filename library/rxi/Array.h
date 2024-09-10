/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

// based on: rxi/vec
// modifications: formatting, names, int -> size_t
#ifndef Array_H
#define Array_H
#include <stdlib.h>
#include <string.h>

#define VEC_VERSION "0.2.1"
#define Array_unpack_(v) \
  (char**)&(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data)
#define Array_t(T)          \
  struct {                  \
      T* data;              \
      int length, capacity; \
  }
#define Array_init(v) memset((v), 0, sizeof(*(v)))
#define Array_deinit(v) (free((v)->data), Array_init(v))
#define Array_push(v, val) \
  (Array_expand_(Array_unpack_(v)) ? -1 : ((v)->data[(v)->length++] = (val), 0), 0)
#define Array_pop(v) (v)->data[--(v)->length]
#define Array_splice(v, start, count) \
  (Array_splice_(Array_unpack_(v), start, count), (v)->length -= (count))
#define Array_swapsplice(v, start, count) \
  (Array_swapsplice_(Array_unpack_(v), start, count), (v)->length -= (count))
#define Array_insert(v, idx, val)                                           \
  (Array_insert_(Array_unpack_(v), idx) ? -1 : ((v)->data[idx] = (val), 0), \
   (v)->length++,                                                           \
   0)
#define Array_sort(v, fn) qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)
#define Array_swap(v, idx1, idx2) Array_swap_(Array_unpack_(v), idx1, idx2)
#define Array_truncate(v, len) ((v)->length = (len) < (v)->length ? (len) : (v)->length)
#define Array_clear(v) ((v)->length = 0)
#define Array_first(v) (v)->data[0]
#define Array_last(v) (v)->data[(v)->length - 1]
#define Array_reserve(v, n) Array_reserve_(Array_unpack_(v), n)
#define Array_compact(v) Array_compact_(Array_unpack_(v))
#define Array_pusharr(v, arr, count)                                  \
  do {                                                                \
    int i__, n__ = (count);                                           \
    if (Array_reserve_po2_(Array_unpack_(v), (v)->length + n__) != 0) \
      break;                                                          \
    for (i__ = 0; i__ < n__; i__++) {                                 \
      (v)->data[(v)->length++] = (arr)[i__];                          \
    }                                                                 \
  } while (0)
#define Array_extend(v, v2) Array_pusharr((v), (v2)->data, (v2)->length)
#define Array_find(v, val, idx)                     \
  do {                                              \
    for ((idx) = 0; (idx) < (v)->length; (idx)++) { \
      if ((v)->data[(idx)] == (val))                \
        break;                                      \
    }                                               \
    if ((idx) == (v)->length)                       \
      (idx) = -1;                                   \
  } while (0)
#define Array_remove(v, val)     \
  do {                           \
    int idx__;                   \
    Array_find(v, val, idx__);   \
    if (idx__ != -1)             \
      Array_splice(v, idx__, 1); \
  } while (0)
#define Array_reverse(v)                             \
  do {                                               \
    int i__ = (v)->length / 2;                       \
    while (i__--) {                                  \
      Array_swap((v), i__, (v)->length - (i__ + 1)); \
    }                                                \
  } while (0)
#define Array_foreach(v, var, iter) \
  if ((v)->length > 0)              \
    for ((iter) = 0; (iter) < (v)->length && (((var) = (v)->data[(iter)]), 1); ++(iter))
#define Array_foreach_rev(v, var, iter)                                             \
  if ((v)->length > 0)                                                              \
    for ((iter) = (v)->length - 1; (iter) >= 0 && (((var) = (v)->data[(iter)]), 1); \
         --(iter))
#define Array_foreach_ptr(v, var, iter) \
  if ((v)->length > 0)                  \
    for ((iter) = 0; (iter) < (v)->length && (((var) = &(v)->data[(iter)]), 1); ++(iter))
#define Array_foreach_ptr_rev(v, var, iter)                                          \
  if ((v)->length > 0)                                                               \
    for ((iter) = (v)->length - 1; (iter) >= 0 && (((var) = &(v)->data[(iter)]), 1); \
         --(iter))
int Array_expand_(char** data, int* length, int* capacity, int memsz);
int Array_reserve_(char** data, int* length, int* capacity, int memsz, int n);
int Array_reserve_po2_(char** data, int* length, int* capacity, int memsz, int n);
int Array_compact_(char** data, int* length, int* capacity, int memsz);
int Array_insert_(char** data, int* length, int* capacity, int memsz, int idx);
void Array_splice_(char** data, int* length, int* capacity, int memsz, int start, int count);
void Array_swapsplice_(
  char** data,
  int* length,
  int* capacity,
  int memsz,
  int start,
  int count);
void Array_swap_(char** data, int* length, int* capacity, int memsz, int idx1, int idx2);

#endif
