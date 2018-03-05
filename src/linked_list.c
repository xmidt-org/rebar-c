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


#include "rebar-c.h"

static tvall_iterator_response_t __find_iterator( tvall_s_node_t *node,
                                                  void *user_data );


void tvall_s_insert( tvall_s_list_t *list, tvall_s_node_t *new_node,
                     tvall_s_node_t *insert_near_node,
                     const tvall_insert_mode_t mode )
{
    tvall_s_node_t *current, *prev;

    if( (NULL == new_node) || (NULL == insert_near_node) ) {
        return;
    }

    prev = NULL;
    current = list->head;

    if( (insert_near_node == current) && (TVALL_MODE__BEFORE == mode) ) {
        tvall_s_prepend( list, new_node );
        return;
    }

    if( (insert_near_node == list->tail) && (TVALL_MODE__AFTER == mode) ) {
        tvall_s_append( list, new_node );
        return;
    }

    /* We are inserting somewhere in the middle of the list,
     * so no head or tail pointers need to be updated. */

    while( NULL != current ) {
        if( insert_near_node == current ) {
            /* We found the node to insert next to. */
            if( TVALL_MODE__BEFORE == mode ) {
                prev->next = new_node;
                new_node->next = current;
            } else {
                new_node->next = current->next;
                current->next = new_node;
            }
            return;
        }

        prev = current;
        current = current->next;
    }
}


void tvall_s_append( tvall_s_list_t *list, tvall_s_node_t *node )
{
    if( NULL != node ) {
        if( NULL != list->tail ) {
            /* This is easy - add to the end of */
            /* the existing list. */
            list->tail->next = node;
            list->tail = node;
        } else {
            /* No list items are present. */
            list->head = node;
            list->tail = node;
        }
        node->next = NULL;
    }
}

size_t tvall_s_count( tvall_s_list_t *list )
{
    size_t count;
    tvall_s_node_t *node;

    count = 0;

    node = tvall_s_get_first( list );
    while( NULL != node ) {
        count++;
        node = node->next;
    }

    return count;
}

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct find_info {
    tvall_s_node_t *found;
    void *needle;
    int offset;
    tvall_s_cmp_node_fn_t cmp_fn;
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static tvall_iterator_response_t __find_iterator( tvall_s_node_t *node,
                                                  void *user_data );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
tvall_s_node_t *__tvall_s_find( tvall_s_list_t *list,
                                tvall_s_cmp_node_fn_t cmp_fn,
                                void *needle,
                                int offset )
{
    struct find_info find;

    find.found = NULL;
    find.needle = needle;
    find.offset = offset;
    find.cmp_fn = cmp_fn;

    tvall_s_iterate(list, __find_iterator, NULL, &find);

    return find.found;
}


void tvall_s_prepend( tvall_s_list_t *list, tvall_s_node_t *node )
{
    if( NULL != node ) {
        node->next = list->head;
        list->head = node;

        if( NULL == list->tail ) {
            list->tail = node;
        }
    }
}

void tvall_s_remove( tvall_s_list_t *list,
                     tvall_s_node_t *node )
{
    tvall_s_node_t *current, *prev;

    prev = NULL;
    current = tvall_s_get_first( list );

    while( NULL != current ) {

        if( current == node ) {
            if( NULL != prev ) {
                prev->next = current->next;
            } else {
                /* We removed from the head of the list. */
                list->head = current->next;
            }

            if( NULL == current->next ) {
                /* This current is at the end of the list. */
                list->tail = prev;
            }

            return;
        }

        prev = current;
        current = current->next;
    }
}

void tvall_s_iterate( tvall_s_list_t *list,
                      tvall_s_iterator_fn_t iterator,
                      tvall_s_delete_node_fn_t deleter,
                      void *user_data )
{
    tvall_s_node_t *node, *prev, *next;

    prev = NULL;
    node = tvall_s_get_first( list );

    while( NULL != node ) {
        tvall_iterator_response_t response;

        next = node->next;

        response = TVALL_IR__DELETE_AND_CONTINUE;

        if( NULL != iterator ) {
            response = (*iterator)( node, user_data );
        }

        /* Do we want to delete the node? */
        if( (TVALL_IR__DELETE_AND_CONTINUE == response) ||
            (TVALL_IR__DELETE_AND_STOP == response) )
        {
            /* Yep */

            if( NULL != prev ) {
                prev->next = node->next;
            } else {
                /* We removed from the head of the list. */
                list->head = node->next;
            }

            if( NULL == node->next ) {
                /* This node is at the end of the list. */
                list->tail = prev;
            }

            if( NULL != deleter ) {
                (*deleter)( node, user_data );
            }

            if( TVALL_IR__DELETE_AND_STOP == response ) {
                return;
            }
            /* The prev node doesn't move because we got rid of the
             * node that would have been the previous node. */
        } else {
            if( TVALL_IR__STOP == response ) {
                return;
            }
            prev = node;
        }

        node = next;
    }
}

void tvall_s_iterate_from(tvall_s_node_t *from_node,
                          tvall_s_iterator_fn_t iterator,
                          void *user_data)
{
    tvall_s_node_t *node, *next;

    if( NULL == from_node )
    {
        return;
    }

    node = from_node->next;

    while( NULL != node ) {
        tvall_iterator_response_t response;

        next = node->next;
        response = TVALL_IR__STOP;

        if( NULL != iterator ) {
            response = (*iterator)( node, user_data );
        }

        /* Do we want to delete the node? */
        if( TVALL_IR__STOP == response ) {
            return;
        }
        node = next;
    }
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Finds the matching structure and passes the pointer back using
 *  the user_data node data.
 *
 *  @param node the node being examined during the iterator
 *  @param user_data the struct find_info* we passed in
 *
 *  @returns TVALL_IR__CONTINUE if it is not a match,
 *           TVALL_IR__STOP when a match is found
 */
static tvall_iterator_response_t __find_iterator( tvall_s_node_t *node,
                                                  void *user_data )
{
    struct find_info *find;
    void *current;

    find = (struct find_info*) user_data;
    current = ((void*)((char *)node - find->offset));

    if (0 == (*find->cmp_fn)(find->needle, current)) {
        find->found = node;
        return TVALL_IR__STOP;
    }

    return TVALL_IR__CONTINUE;
}
