/*
 *  Copyright (c) 2012  Comcast
 */

#include <stdlib.h>
#include <assert.h>

#include "cvs-hashmap.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef int (*cvs_hashmap_key_cmp_fct)(void *key_a, void *key_b);

typedef struct {
    rebar_ll_node_t node;
    union {
        char *string;
        uint32_t u32;
#ifdef SUPPORT_UINT64
        uint64_t u64;
#endif
    } key;
    void *value;
} cvs_hashmap_node_t;

typedef struct {
    cvs_hashmap_iterator_fn_t fn;
    cvs_hashmap_key_cmp_fct cmp;
    void *user_data;
} cvs_hashmap_iterator_helper_t;

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void __deleter(rebar_ll_node_t *node, void *user_data);
static cvs_hashmap_node_t *__get(cvs_hashmap_t *hashmap, void *key);
static rebar_ll_iterator_response_t __hash_iterator(rebar_ll_node_t *node, void *user_data);
static int __string_cmp(void *key_a, void *key_b);
static int __uint32_t_cmp(void *key_a, void *key_b);
#ifdef SUPPORT_UINT64
static int __uint64_t_cmp(void *key_a, void *key_b);
#endif

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/* See cvs-hashmap.h for details. */
bool cvs_hashmap_init(cvs_hashmap_t *hashmap, cvs_hashmap_type_t type)
{
    bool rv;

    rv = false;
    if (hashmap) {
        rebar_ll_init(&hashmap->list);

        rv = true;
        switch( type ) {
            case CHT__STRING:
                hashmap->cmp = __string_cmp;
                break;

            case CHT__UINT32:
                hashmap->cmp = __uint32_t_cmp;
                break;

#ifdef SUPPORT_UINT64
            case CHT__UINT64:
                hashmap->cmp = __uint64_t_cmp;
                break;
#endif

            default:
                rv = false;
                break;
        }
    }

    return rv;
}


/* See cvs-hashmap.h for details. */
void cvs_hashmap_destroy(cvs_hashmap_t *hashmap)
{
    if (hashmap) {
        rebar_ll_delete_all(&hashmap->list, __deleter, NULL);
    }
}


/* See cvs-hashmap.h for details. */
void *cvs_hashmap_get(cvs_hashmap_t *hashmap, void *key)
{
    void *rv;
    cvs_hashmap_node_t *n;

    rv = NULL;
    n = __get(hashmap, key);
    if (n) {
        rv = n->value;
    }

    return rv;
}


/* See cvs-hashmap.h for details. */
bool cvs_hashmap_contains_key(cvs_hashmap_t *hashmap, void *key)
{
    return (NULL != __get(hashmap, key)) ? true : false;
}


/* See cvs-hashmap.h for details. */
void *cvs_hashmap_remove(cvs_hashmap_t *hashmap, void *key)
{
    void *rv;
    cvs_hashmap_node_t *n;

    rv = NULL;
    n = __get(hashmap, key);
    if (n) {
        rv = n->value;
        rebar_ll_remove(&hashmap->list, &n->node);
        free(n);
    }

    return rv;
}


/* See cvs-hashmap.h for details. */
void cvs_hashmap_put(cvs_hashmap_t *hashmap, void *key, void *value)
{
    cvs_hashmap_node_t *n;

    n = __get(hashmap, key);
    if (!n) {
        n = (cvs_hashmap_node_t *) malloc(sizeof(cvs_hashmap_node_t));
	assert(n);
        if (__string_cmp == hashmap->cmp) {
            n->key.string = key;
#ifdef SUPPORT_UINT64
        } else if (__uint64_t_cmp == hashmap->cmp) {
            n->key.u64 = *((uint64_t*) key);
#endif
        } else {    /* uint32_t mode */
            n->key.u32 = *((uint32_t*) key);
        }
    }
    n->value = value;
    rebar_ll_append(&hashmap->list, &n->node);
}


/* See cvs-hashmap.h for details. */
bool cvs_hashmap_is_empty(cvs_hashmap_t *hashmap)
{
    return (0 == cvs_hashmap_get_size(hashmap)) ? true : false;
}


/* See cvs-hashmap.h for details. */
void cvs_hashmap_iterate(cvs_hashmap_t *hashmap,
                         cvs_hashmap_iterator_fn_t iterator,
                         void *user_data)
{
    if (hashmap) {
        cvs_hashmap_iterator_helper_t h;

        h.cmp = (cvs_hashmap_key_cmp_fct) hashmap->cmp;
        h.fn = iterator;
        h.user_data = user_data;

        rebar_ll_iterate(&hashmap->list, __hash_iterator, NULL, &h);
    }
}


/* See cvs-hashmap.h for details. */
size_t cvs_hashmap_get_size(cvs_hashmap_t *hashmap)
{
    size_t rv;

    rv = 0;
    if (hashmap) {
        rv = rebar_ll_count(&hashmap->list);
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Deletes the node from the list.
 *
 *  @param node the node being deleted
 *  @param user_data the destructor function to call
 */
static void __deleter(rebar_ll_node_t *node, void *user_data)
{
    cvs_hashmap_node_t *n;

    (void ) user_data; /* Unused */

    n = rebar_ll_get_data(cvs_hashmap_node_t, node, node);

    free(n);
}


/**
 *  Gets the hashmap node for the specified key or returns NULL.
 *
 *  @param hashmap the hashmap to process
 *  @param key the key to search for
 *
 *  @return the node of the key or NULL
 */
static cvs_hashmap_node_t *__get(cvs_hashmap_t *hashmap, void *key)
{
    cvs_hashmap_node_t *rv;

    rv = NULL;
    if ((hashmap) && (key)) {
        rebar_ll_node_t *n;
        n = rebar_ll_find(&hashmap->list, hashmap->cmp, key, cvs_hashmap_node_t, node);

        if (n) {
            rv = rebar_ll_get_data(cvs_hashmap_node_t, node, n);
        }
    }

    return rv;
}


/**
 *  Called during the iteration of each hashmap key-value pair by linked list code.
 *
 *  See rebar-c.h for details.
 */
static rebar_ll_iterator_response_t __hash_iterator(rebar_ll_node_t *node, void *user_data)
{
    bool tmp;
    cvs_hashmap_node_t *n;
    cvs_hashmap_iterator_helper_t *h;

    n = rebar_ll_get_data(cvs_hashmap_node_t, node, node);
    h = (cvs_hashmap_iterator_helper_t*) user_data;

    if (__string_cmp == h->cmp) {
        tmp = (h->fn)(n->key.string, n->value, h->user_data);
#ifdef SUPPORT_UINT64
    } else if (__uint64_t_cmp == h->cmp) {
        tmp = (h->fn)(&n->key.u64, n->value, h->user_data);
#endif
    } else {    /* uint32_t mode */
        tmp = (h->fn)(&n->key.u32, n->value, h->user_data);
    }

    if (tmp) {
        return REBAR_IR__CONTINUE;
    }

    return REBAR_IR__STOP;
}


static int __string_cmp(void *key_a, void *key_b)
{
    cvs_hashmap_node_t *n = (cvs_hashmap_node_t*) key_b;

    return strcmp((char*) key_a, n->key.string);
}


static int __uint32_t_cmp(void *key_a, void *key_b)
{
    cvs_hashmap_node_t *n;
    uint32_t a;

    a = *((uint32_t*) key_a);
    n = (cvs_hashmap_node_t*) key_b;

    if (a == n->key.u32) {
        return 0;
    }
    if (a < n->key.u32) {
        return -1;
    }

    return 1;
}


#ifdef SUPPORT_UINT64
static int __uint64_t_cmp(void *key_a, void *key_b)
{
    cvs_hashmap_node_t *n;
    uint64_t a;

    a = *((uint64_t*) key_a);
    n = (cvs_hashmap_node_t*) key_b;

    if (a == n->key.u64) {
        return 0;
    }
    if (a < n->key.u64) {
        return -1;
    }

    return 1;
}
#endif
