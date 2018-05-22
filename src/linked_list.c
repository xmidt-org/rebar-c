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

static rebar_ll_iterator_response_t __find_iterator( rebar_ll_node_t *node,
                                                  void *user_data );


void rebar_ll_insert( rebar_ll_list_t *list, rebar_ll_node_t *new_node,
                     rebar_ll_node_t *insert_near_node,
                     const rebar_ll_insert_mode_t mode )
{
    rebar_ll_node_t *current, *prev;

    if( (NULL == new_node) || (NULL == insert_near_node) ) {
        return;
    }

    prev = NULL;
    current = list->head;

    if( (insert_near_node == current) && (REBAR_MODE__BEFORE == mode) ) {
        rebar_ll_prepend( list, new_node );
        return;
    }

    if( (insert_near_node == list->tail) && (REBAR_MODE__AFTER == mode) ) {
        rebar_ll_append( list, new_node );
        return;
    }

    /* We are inserting somewhere in the middle of the list,
     * so no head or tail pointers need to be updated. */

    while( NULL != current ) {
        if( insert_near_node == current ) {
            /* We found the node to insert next to. */
            if( REBAR_MODE__BEFORE == mode ) {
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


void rebar_ll_append( rebar_ll_list_t *list, rebar_ll_node_t *node )
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

size_t rebar_ll_count( rebar_ll_list_t *list )
{
    size_t count;
    rebar_ll_node_t *node;

    count = 0;

    node = rebar_ll_get_first( list );
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
    rebar_ll_node_t *found;
    void *needle;
    int offset;
    rebar_ll_cmp_node_fn_t cmp_fn;
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static rebar_ll_iterator_response_t __find_iterator( rebar_ll_node_t *node,
                                                  void *user_data );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
rebar_ll_node_t *__rebar_ll_find( rebar_ll_list_t *list,
                                rebar_ll_cmp_node_fn_t cmp_fn,
                                void *needle,
                                int offset )
{
    struct find_info find;

    find.found = NULL;
    find.needle = needle;
    find.offset = offset;
    find.cmp_fn = cmp_fn;

    rebar_ll_iterate(list, __find_iterator, NULL, &find);

    return find.found;
}


void rebar_ll_prepend( rebar_ll_list_t *list, rebar_ll_node_t *node )
{
    if( NULL != node ) {
        node->next = list->head;
        list->head = node;

        if( NULL == list->tail ) {
            list->tail = node;
        }
    }
}

void rebar_ll_remove( rebar_ll_list_t *list,
                     rebar_ll_node_t *node )
{
    rebar_ll_node_t *current, *prev;

    prev = NULL;
    current = rebar_ll_get_first( list );

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

int rebar_ll_iterate( rebar_ll_list_t *list,
                      rebar_ll_iterator_fn_t iterator,
                      rebar_ll_delete_node_fn_t deleter,
                      void *user_data )
{
    rebar_ll_node_t *node, *prev, *next;
    int deleter_called = 0;

    prev = NULL;
    node = rebar_ll_get_first( list );

    while( NULL != node ) {
        rebar_ll_iterator_response_t response;

        next = node->next;

        response = REBAR_IR__DELETE_AND_CONTINUE;

        if( NULL != iterator ) {
            response = (*iterator)( node, user_data );
        }

        /* Do we want to delete the node? */
        if( (REBAR_IR__DELETE_AND_CONTINUE == response) ||
            (REBAR_IR__DELETE_AND_STOP == response) )
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
                deleter_called++;
            }

            if( REBAR_IR__DELETE_AND_STOP == response ) {
                return deleter_called;
            }
            /* The prev node doesn't move because we got rid of the
             * node that would have been the previous node. */
        } else {
            if( REBAR_IR__STOP == response ) {
                return deleter_called;
            }
            prev = node;
        }

        node = next;
    }

    return deleter_called;
}

void rebar_ll_iterate_from(rebar_ll_node_t *from_node,
                          rebar_ll_iterator_fn_t iterator,
                          void *user_data)
{
    rebar_ll_node_t *node, *next;

    if( NULL == from_node )
    {
        return;
    }

    node = from_node->next;

    while( NULL != node ) {
        rebar_ll_iterator_response_t response;

        next = node->next;
        response = REBAR_IR__STOP;

        if( NULL != iterator ) {
            response = (*iterator)( node, user_data );
        }

        /* Do we want to delete the node? */
        if( REBAR_IR__STOP == response ) {
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
 *  @returns REBAR_IR__CONTINUE if it is not a match,
 *           REBAR_IR__STOP when a match is found
 */
static rebar_ll_iterator_response_t __find_iterator( rebar_ll_node_t *node,
                                                  void *user_data )
{
    struct find_info *find;
    void *current;

    find = (struct find_info*) user_data;
    current = ((void*)((char *)node - find->offset));

    if (0 == (*find->cmp_fn)(find->needle, current)) {
        find->found = node;
        return REBAR_IR__STOP;
    }

    return REBAR_IR__CONTINUE;
}
