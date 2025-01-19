#ifndef generic_Queue_H_
#define generic_Queue_H_

#define GENERIC_QUEUE_DECLARE(Name, Type, Max)                            \
  typedef struct {                                                        \
      struct {                                                            \
          size_t front;                                                   \
          size_t back;                                                    \
      } index;                                                            \
      size_t count;                                                       \
      Type items[Max];                                                    \
  } Name;                                                                 \
  void Name##_push(Name queue[const static 1], const Type item);          \
  bool Name##_pop(Name queue[const static 1], Type item[const static 1]); \
  void Name##_print(const Name queue);

#define GENERIC_QUEUE_DEFINE(Name, Type, Max)                              \
  void Name##_push(Name queue[const static 1], const Type item) {          \
    if (queue->count >= Max) {                                             \
      fprintf(stderr, "Max queue size exceeded\n");                        \
      abort();                                                             \
    }                                                                      \
    else {                                                                 \
      queue->count++;                                                      \
      queue->items[queue->index.back++] = item;                            \
      if (queue->index.back > Max) {                                       \
        queue->index.back = 0;                                             \
      }                                                                    \
    }                                                                      \
  }                                                                        \
  bool Name##_pop(Name queue[const static 1], Type item[const static 1]) { \
    bool result = false;                                                   \
    if (queue->count) {                                                    \
      *item = queue->items[queue->index.front];                            \
      if (!(--queue->count)) {                                             \
        queue->index.front = 0;                                            \
        queue->index.back = 0;                                             \
      }                                                                    \
      else if (++queue->index.front > Max) {                               \
        queue->index.front = 0;                                            \
      }                                                                    \
      result = true;                                                       \
    }                                                                      \
    return result;                                                         \
  }                                                                        \
  void Name##_print(const Name queue) {                                    \
    printf("queue:\n");                                                    \
    printf("\tfront: %zu\n", queue.index.front);                           \
    printf("\tback: %zu\n", queue.index.back);                             \
    printf("\tcount: %zu\n", queue.count);                                 \
  }

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int example1;
    double example2;
} ExampleItem;
GENERIC_QUEUE_DECLARE(Examples, ExampleItem, 10)
GENERIC_QUEUE_DEFINE(Examples, ExampleItem, 10)

#endif // generic_Queue_H_
