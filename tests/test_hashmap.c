/** File: test_utf8.c
 *
 * Abstract: test driver for unit test of UTF8 utilities.
 *
 * Copyright (c) 2012 Comcast
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdbool.h>

#include "../src/cvs-hashmap.h"
#include "test_hashmap.h"
#include "general.h"

static int __str_iterator_once__count;
bool str_iterator_once(void *key, void *value, void *user_data)
{
    IGNORE_UNUSED(key)
    IGNORE_UNUSED(value)
    IGNORE_UNUSED(user_data)
            
    CU_ASSERT(0 == __str_iterator_once__count);
    __str_iterator_once__count++;

    return false;
}

bool str_iterator(void *key, void *value, void *user_data)
{
    char *k;
    uint32_t v;
    IGNORE_UNUSED(user_data)

    k = CVSHM_KEY_TO_STRING(key);
    v = *((uint32_t*) value);

    if ('f' == *k) {
        CU_ASSERT(12 == v);
    } else if ('c' == *k) {
        CU_ASSERT(10 == v);
    } else if ('d' == *k) {
        CU_ASSERT(11 == v);
    } else {
        CU_ASSERT(0 == 1);
    }

    return true;
}

bool uint64_iterator(void *key, void *value, void *user_data)
{
    uint64_t k;
    uint32_t v;
    IGNORE_UNUSED(user_data)

    k = CVSHM_KEY_TO_UINT64(key);
    v = *((uint32_t*) value);

    if (20 == k) {
        CU_ASSERT(10 == v);
    } else if (21 == k) {
        CU_ASSERT(11 == v);
    } else if (22 == k) {
        CU_ASSERT(12 == v);
    } else {
        CU_ASSERT(0 == 1);
    }

    return true;
}

void simple_string( void )
{
    cvs_hashmap_t hash;
    uint32_t bar[3];

    bar[0] = 10;
    bar[1] = 11;
    bar[2] = 12;

    CU_ASSERT(true  == cvs_hashmap_init(&hash, CHT__STRING));

    CU_ASSERT(true  == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(0     == cvs_hashmap_get_size(&hash));
    CU_ASSERT(NULL  == cvs_hashmap_get(&hash, "foo"));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, "foo"));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, "car"));
    CU_ASSERT(NULL  == cvs_hashmap_remove(&hash, "foo"));

    cvs_hashmap_put(&hash, "foo", &bar[0]);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(1       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[0] == cvs_hashmap_get(&hash, "foo"));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, "foo"));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, "car"));

    CU_ASSERT(&bar[0] == cvs_hashmap_remove(&hash, "foo"));

    CU_ASSERT(true  == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(0     == cvs_hashmap_get_size(&hash));
    CU_ASSERT(NULL  == cvs_hashmap_get(&hash, "foo"));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, "foo"));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, "car"));

    cvs_hashmap_put(&hash, "foo", &bar[0]);
    cvs_hashmap_put(&hash, "foo", &bar[1]);
    cvs_hashmap_put(&hash, "foo", &bar[2]);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(1       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[2] == cvs_hashmap_get(&hash, "foo"));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, "foo"));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, "car"));

    cvs_hashmap_put(&hash, "car", &bar[0]);
    cvs_hashmap_put(&hash, "dog", &bar[1]);

    cvs_hashmap_iterate(&hash, str_iterator, NULL);

    __str_iterator_once__count = 0;
    cvs_hashmap_iterate(&hash, str_iterator_once, NULL);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(3       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[0] == cvs_hashmap_get(&hash, "car"));
    CU_ASSERT(&bar[1] == cvs_hashmap_get(&hash, "dog"));
    CU_ASSERT(&bar[2] == cvs_hashmap_get(&hash, "foo"));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, "foo"));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, "far"));

    cvs_hashmap_destroy(&hash);
}

void simple_uint64( void )
{
    cvs_hashmap_t hash;
    uint64_t keys[4];
    uint64_t bar[3];

    keys[0] = 20;
    keys[1] = 21;
    keys[2] = 22;
    keys[3] = 23;

    bar[0] = 10;
    bar[1] = 11;
    bar[2] = 12;

    CU_ASSERT(true  == cvs_hashmap_init(&hash, CHT__UINT64));

    CU_ASSERT(true  == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(0     == cvs_hashmap_get_size(&hash));
    CU_ASSERT(NULL  == cvs_hashmap_get(&hash, &keys[0]))
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, &keys[0]))
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, &keys[1]))
    CU_ASSERT(NULL  == cvs_hashmap_remove(&hash, &keys[0]));

    cvs_hashmap_put(&hash, &keys[0], &bar[0]);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(1       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[0] == cvs_hashmap_get(&hash, &keys[0]));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, &keys[0]));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, &keys[1]));

    CU_ASSERT(&bar[0] == cvs_hashmap_remove(&hash, &keys[0]));

    CU_ASSERT(true  == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(0     == cvs_hashmap_get_size(&hash));
    CU_ASSERT(NULL  == cvs_hashmap_get(&hash, &keys[0]));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, &keys[0]));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, &keys[1]));

    cvs_hashmap_put(&hash, &keys[2], &bar[0]);
    cvs_hashmap_put(&hash, &keys[2], &bar[1]);
    cvs_hashmap_put(&hash, &keys[2], &bar[2]);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(1       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[2] == cvs_hashmap_get(&hash, &keys[2]));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, &keys[2]));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, &keys[0]));

    cvs_hashmap_put(&hash, &keys[0], &bar[0]);
    cvs_hashmap_put(&hash, &keys[1], &bar[1]);

    cvs_hashmap_iterate(&hash, uint64_iterator, NULL);

    CU_ASSERT(false   == cvs_hashmap_is_empty(&hash));
    CU_ASSERT(3       == cvs_hashmap_get_size(&hash));
    CU_ASSERT(&bar[0] == cvs_hashmap_get(&hash, &keys[0]));
    CU_ASSERT(&bar[1] == cvs_hashmap_get(&hash, &keys[1]));
    CU_ASSERT(&bar[2] == cvs_hashmap_get(&hash, &keys[2]));
    CU_ASSERT(true    == cvs_hashmap_contains_key(&hash, &keys[0]));
    CU_ASSERT(false   == cvs_hashmap_contains_key(&hash, &keys[3]));

    cvs_hashmap_destroy(&hash);
}

void boundary(void)
{
    cvs_hashmap_t hash;

    CU_ASSERT(true  == cvs_hashmap_init(&hash, CHT__STRING));

    CU_ASSERT(false == cvs_hashmap_init(NULL, CHT__STRING));
    CU_ASSERT(false == cvs_hashmap_init(&hash, (cvs_hashmap_type_t) 99));

    cvs_hashmap_destroy(NULL);
    cvs_hashmap_iterate(NULL, NULL, NULL);
    CU_ASSERT(0     == cvs_hashmap_get_size(NULL));
    CU_ASSERT(false == cvs_hashmap_contains_key(NULL, NULL));
    CU_ASSERT(false == cvs_hashmap_contains_key(&hash, NULL));

    cvs_hashmap_destroy(&hash);
}


void add_hashmap_tests(CU_pSuite *suite)
{
    CU_add_test(*suite, "hashmap string", simple_string);
    CU_add_test(*suite, "hashmap uint64_t", simple_uint64);
    CU_add_test(*suite, "hashmap Boundary tests ", boundary);
}
 