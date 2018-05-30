#include <stddef.h>
#include <stdbool.h>
#ifndef __QUEUE_INTERNAL_H__
#define __QUEUE_INTERNAL_H__


typedef struct _queue_element_type {
void *data;
struct _queue_element_t *next;
struct _queue_element_t *prev;
} queue_element_t;

typedef struct queue_internal_t {
queue_element_t *head;
queue_element_t *tail;
size_t current_size;
bool syncronized; // optional ??

// etc ...
} queue_internal_struct_type;

#endif
