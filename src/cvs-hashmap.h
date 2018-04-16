/*
 *  Copyright (c) 2012  Comcast
 */
#ifndef __CVS_HASHMAP_H__
#define __CVS_HASHMAP_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "rebar-c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define CVSHM_KEY_TO_STRING(key)    ((char*) key)
#define CVSHM_KEY_TO_UINT32(key)    (*((uint32_t*) key))
#define CVSHM_KEY_TO_UINT64(key)    (*((uint64_t*) key))

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef enum {
    CHT__STRING,
    CHT__UINT32,
    CHT__UINT64
} cvs_hashmap_type_t;


/**
 *  Called during the iterate operation for each key-value pair.
 *
 *  @note No hash manipulation is permitted during this call.
 *
 *  @param key the pointer to the key portion of the map
 *  @param value the value portion of the map
 *
 *  @return true to continue, false stops the iteration process
 */
typedef bool (*cvs_hashmap_iterator_fn_t)(void *key, void *value, void *user_data);


/* Do not directly access any of the values in the structure. */
typedef struct {
    rebar_ll_list_t list;
    void *cmp;
} cvs_hashmap_t;

/*----------------------------------------------------------------------------*/
/*                                 Functions                                  */
/*----------------------------------------------------------------------------*/

/**
 *  Initializes the hashmap structure.
 *
 *  @param hashmap the hashmap to initialize
 *
 *  @return true if successful, false otherwise
 */
bool cvs_hashmap_init(cvs_hashmap_t *hashmap, cvs_hashmap_type_t type );


/**
 *  Destroys the structure.
 *
 *  @note This does not destroy the key-value pairs of the hashmap, only
 *        the hashmap.
 *
 *  @param hashmap the hashmap to destroy
 */
void cvs_hashmap_destroy(cvs_hashmap_t *hashmap);


/**
 *  Returns the value to which the specified key is mapped, or NULL if this map
 *  contains no mapping for the key.
 *
 *  @param hashmap the hashmap to search
 *  @param key the pointer to the key whose associated value is to be returned
 *
 *  @return the value to which the specified key is mapped, or NULL if this map
 *          contains no mapping for the key (or any other error occurs)
 */
void *cvs_hashmap_get(cvs_hashmap_t *hashmap, void *key);


/**
 *  Returns true if this map contains a mapping for the specified key.
 *
 *  @param hashmap the hashmap to search
 *  @param key the pointer to the key whose associated value is to be tested
 *
 *  @return true if this map contains a mapping for the specified key, or false
 *          otherwise
 */
bool cvs_hashmap_contains_key(cvs_hashmap_t *hashmap, void *key);


/**
 *  Removes the mapping for the specified key from this map if present.
 *
 *  @param hashmap the hashmap to search
 *  @param key the pointer to the key whose mapping is to be removed from the map
 *
 *  @return the previous value associated with key, or NULL if there was no
 *          mapping for key (or any other error occurs)
 */
void *cvs_hashmap_remove(cvs_hashmap_t *hashmap, void *key);


/**
 *  Associates the specified value with the specified key in this map. If the
 *  map previously contained a mapping for the key, the old value is replaced. 
 *
 *  @param hashmap the hashmap to search
 *  @param key pointer to the key with which the specified value is to be associated
 *  @param value value to be associated with the specified key
 */
void cvs_hashmap_put(cvs_hashmap_t *hashmap, void *key, void *value);


/**
 *  Returns true if this map contains no key-value mappings.
 *
 *  @param hashmap the hashmap to search
 *
 *  @return true if this map contains no key-value mappings
 */
bool cvs_hashmap_is_empty(cvs_hashmap_t *hashmap);


/**
 *  Iterates over the list of key-value mappings and calls the provided iterator
 *  function for each pair.
 *
 *  @param hashmap the hashmap to iterate over
 *  @param iterator the iterator function to call for each pair
 *  @param user_data additional user data passed through to the iterator function
 */
void cvs_hashmap_iterate(cvs_hashmap_t *hashmap,
                         cvs_hashmap_iterator_fn_t iterator,
                         void *user_data);


/**
 *  Returns the number of key-value mappings in this hashmap. 
 *
 *  @param hashmap the hashmap to inspect
 *
 *  @return the number of key-value mappings in the hashmap, or 0 on error
 */
size_t cvs_hashmap_get_size(cvs_hashmap_t *hashmap);


#ifdef __cplusplus
}
#endif
#endif

