#include <stddef.h>
#include <stdbool.h>

#ifndef __QUEUE_H__
#define __QUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct queue_internal_t queue_t;
    typedef void (*rebar_queue_delete_element_fn_t) (void *user_data);

    /*
     */
    queue_t *rebar_queue_init(void);

    /*
     * Param: deleter  function to call to delete user data.
     * If NULL is specified user data will be leaked.
     */
    int rebar_queue_delete(queue_t *, rebar_queue_delete_element_fn_t deleter);

    /*
     */
    void * rebar_queue_pop(queue_t *);

    /*
     */
    int rebar_queue_push(void *, queue_t *);

    /*
     */
    void * rebar_queue_peek(queue_t *);

    /*
     */
    size_t rebar_queue_size(queue_t *);

    /*
     */
    bool rebar_queue_is_empty(queue_t *);

    /*
     Queue needs to know the length of data to print it.
     */
    void rebar_queue_print(queue_t *, const size_t length);

#ifdef __cplusplus
}
#endif

#endif

