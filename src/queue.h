#include <stddef.h>
#include <stdbool.h>

#ifndef __QUEUE_H__
#define __QUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef  struct queue_internal_t  queue_t;
/*
 */
queue_t *init_queue (void);

/*
 */
int    delete_queue(queue_t);

/*
 */
void * pop  (queue_t);

/*
 */
int    push  (void *, queue_t);

/*
 */
void * peek  (queue_t);

/*
 */
size_t size  (queue_t);



#ifdef __cplusplus
}
#endif

#endif

