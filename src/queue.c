/**
 * Copyright 2018 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "queue_internal.h"
#include "queue.h"
#include "rebar-xxd.h"
/*
 queue.h implements a singly linked list FIFO
 */

/*
 * Allocates memory for the queue, queue must be deleted by
 * calling rebar_queue_delete(), all client data must have been 
 * freed by the user of this API, Queue will only free it's own structures.
 * Will assert in os if malloc() fails!
 */
queue_t *rebar_queue_init (void)
{
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    memset(q, 0, sizeof(queue_t));

    return q;
}

/*
 */
int rebar_queue_delete (queue_t *q)
{
    queue_element_t *e;
    
    if (NULL == q) {
        return -1;
    }
    
    e = q->head;

    if (q->current_size) {
        do {
          queue_element_t *next = e->next;
          rebar_queue_pop(q);
          e = next;
        } while (e != q->tail);
    }
    free(q);
    return 0;
}

/*
 * returns and removes top most (head or oldest item) on the Queue.
 */
void *rebar_queue_pop (queue_t *q)
{
    void *data = NULL;
    queue_element_t *e;

    if (q == NULL || q->head == NULL) {
        return data;
    }

    data = q->head->data;
    e = q->head;

    q->head = q->head->next;
    free(e);
    q->current_size--;
    if (0 == q->current_size) {
        q->tail = NULL;
    }
    return data;
}

/*
 * pushes the item to the end (tail) of the queue,
 */
int rebar_queue_push (void *data, queue_t *q)
{
    queue_element_t *e;

    if (NULL == q) {
        return -1;
    }

    e = (queue_element_t *) malloc(sizeof(queue_element_t));
    e->data = data;
    if (0 == q->current_size) { // Empty queue
        q->tail = e;
        q->head = e;
    } else {
        q->tail->next = e;
        q->tail = e;
    }
    e->next = NULL;
    q->current_size++;

    return 0;
}

/*
 * returns "data" for the head, the queue is not affected by this call.
 */
void *rebar_queue_peek (queue_t *q)
{

    if (q->head == NULL) {
        return NULL;
    }

    return q->head->data;
}

/*
 * returns number of items in the Queue
 */
size_t rebar_queue_size (queue_t *q)
{
    if (q) {
        return q->current_size;
    } else {
        return 0;
    }
}

/*
 */
bool rebar_queue_is_empty(queue_t *q)
{
    if (q) {
        return (0 == q->current_size);
    } else {
        return true;
    }
}

/*
 * prints length number of bytes (xxd) per link in the Queue
 * "data" is opaque to the queue.
 */
void rebar_queue_print(queue_t *q, const size_t length)
{
    queue_element_t *e;

    if (NULL == q) {
        return;
    }

    e = q->head;

    if (q->current_size) {
        do {
          rebar_xxd( e->data, length, 80, true );
          e = e->next;
        } while (e != q->tail);
    }
}