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


#ifndef __REBAR_H__
#define __REBAR_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------*/
/*                        Used by all Linked List Types                       */
/*----------------------------------------------------------------------------*/
typedef enum {
    REBAR_MODE__BEFORE,
    REBAR_MODE__AFTER
} rebar_ll_insert_mode_t;

typedef enum {
    REBAR_IR__CONTINUE,
    REBAR_IR__DELETE_AND_CONTINUE,
    REBAR_IR__STOP,
    REBAR_IR__DELETE_AND_STOP
} rebar_ll_iterator_response_t;

/**
 *  Macro to obtain the pointer to your struct given a linked list node
 *  (doesn't matter if it is a rebar_ll_node_t or rebar_ll_d_node_t (when
 *  they exist).
 *
 *  struct myStruct {
 *      char          *junk1;
 *      int            junk2;
 *      rebar_ll_node_t link;
 *  };
 *
 *  rebar_ll_node_t  *joe = rebar_ll_get_first(list);
 *  struct myStruct *lou = MACLISTSTRUCTPTR( struct myStruct, link, joe );
 *
 *  @param struct_name the user structure name
 *  @param node_name the name of the linked list node name
 *  @param node the node to get the user data for
 *
 *  @return the struct_name cast user data pointer
 */
 // Note: The C++ standard states that 'offsetof' is not guaranteed to work
 //  except when the structure is C-compatible POD (Plain Old Data).  IE, when
 //  the structure involves C++ classes, then a warning may occur.
 //  To disable the warning, use the -Wno-invalid-offsetof compiler option. 
#define rebar_ll_get_data( struct_name, node_name, node ) \
    ((struct_name *)((char *)node - offsetof(struct_name, node_name)))

/*----------------------------------------------------------------------------*/
/*                             Singly Linked List                             */
/*----------------------------------------------------------------------------*/

/* Do not directly use this structure's internals.  Only use this library
 * to modify the linked list. */
typedef struct __rebar_ll_node {
    struct __rebar_ll_node *next;
} rebar_ll_node_t;

/* Do not directly use this structure's internals.  Only use this library
 * to modify the linked list. */
typedef struct {
    rebar_ll_node_t *head; 
    rebar_ll_node_t *tail; 
} rebar_ll_list_t;

/**
 *  Called during the list iterate operation for each node.
 *
 *  @note No list manipulation is permitted during this call.
 *
 *  @param node the current node in the iteration over the linked list
 *  @param user_data the supplied user data to the iterator function
 *
 *  @retval REBAR_IR__CONTINUE do not alter the node & continue iterating 
 *  @retval REBAR_IR__DELETE_AND_CONTINUE delete the node & continue
 *                                        iterating 
 *  @retval REBAR_IR__STOP do not alter the node but stop processing
 *  @retval REBAR_IR__DELETE_AND_STOP delete the node & stop processing
 */
typedef rebar_ll_iterator_response_t (*rebar_ll_iterator_fn_t) ( rebar_ll_node_t *node,
                                                             void *user_data );

/**
 *  Called during the list iterate operation when a node has been
 *  marked for deletion, or during the list delete operation.
 *
 *  This is the chance the user has to free any memory associated
 *  with this node (even the node itself).
 *
 *  @param node the node being deleted from the list
 *  @param user_data the supplied user data to the iterator/delete function
 */
typedef void (*rebar_ll_delete_node_fn_t) ( rebar_ll_node_t *node,
                                           void *user_data );

/**
 *  Used to determine if the needle matches the node, or if one is greater
 *  then the other.
 *
 *  @param needle what is being looked for
 *  @param node the node presently being examined
 *
 *  @return 0 if the two are equal,
 *          less then 0 if needle is less then node,
 *          greater then 0 if needle is greater then node
 */
typedef int (*rebar_ll_cmp_node_fn_t) ( void *needle, void *node );

/**
 *  Used to initialize a singly linked list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @param list the pointer to the rebar_ll_list_t struct to initiate
 */
#define rebar_ll_init( list )    \
{                               \
    (list)->head = NULL;        \
    (list)->tail = NULL;        \
}

/**
 *  Used to get the first node from the list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @param list the list to get the first node from
 *
 *  @return first node on success, NULL on error or empty list
 */
#define rebar_ll_get_first( list ) ((list)->head)

/**
 *  Used to get the next node from the list.
 *
 *  @param node the current node of interest
 *
 *  @return next node on success, NULL on error or no further nodes available
 */
#define rebar_ll_get_next( node ) \
    (NULL == (node)) ? NULL : (node)->next

/**
 *  Used to get the last node from the list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @param list the list to get the last node from
 *
 *  @return last node on success, NULL on error or empty list
 */
#define rebar_ll_get_last( list ) ((list)->tail)

/**
 *  Used to append a node to the tail of a list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note This function will allow you to add the same node multiple 
 *        times - DO NOT DO THIS!
 *
 *  @param list the list to append to
 *  @param node the node to append to the list
 */
void rebar_ll_append( rebar_ll_list_t *list, rebar_ll_node_t *node );

/**
 *  Used to prepend a node to the head of the list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note This function will allow you to add the same node multiple 
 *        times - DO NOT DO THIS!
 *
 *  @param list the list to prepend to
 *  @param node the node to prepend to the list
 */
void rebar_ll_prepend( rebar_ll_list_t *list, rebar_ll_node_t *node );

/**
 *  Used to insert a node either before or after the specified
 *  node.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note This function will allow you to add the same node multiple 
 *        times - DO NOT DO THIS!
 *
 *  @param list the list to insert a node into
 *  @param node the node to insert
 *  @param insert_near_node the node to insert the new node either
 *                          before or after
 *  @param mode indicator of the insertion type
 */
void rebar_ll_insert( rebar_ll_list_t *list, rebar_ll_node_t *node,
                     rebar_ll_node_t *insert_near_node,
                     const rebar_ll_insert_mode_t mode );

/**
 *  Used to remove the node at the head of the list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note No memory is released in this operation.  It is up to
 *  the caller of this function to make sure they have a copy
 *  of the head node pointer to prevent memory leaks.
 *
 *  @param list the list whose head not to remove
 */
#define rebar_ll_remove_head( list )         \
{                                           \
    if( NULL != (list)->head ) {            \
        (list)->head = (list)->head->next;  \
        if( NULL == (list)->head ) {        \
            (list)->tail = NULL;            \
        }                                   \
    }                                       \
}

/**
 *  Used to remove an arbitrary node from a list.
 *
 *  @note No memory is released in this operation.  It is up to
 *  the caller of this function to make sure they have a copy
 *  of the head node pointer to prevent memory leaks.
 *
 *  @param list the list to remove a node from
 *  @param node the node to remove from the list
 */
void rebar_ll_remove( rebar_ll_list_t *list, rebar_ll_node_t *node );

/**
 *  Used to iterate over a list and optionally delete nodes from
 *  the list during the iteration.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note No memory is released in this operation.  It is up to
 *  the caller of this function to make sure they have a copy
 *  of the head node pointer to prevent memory leaks.
 *
 *  @param list the list to iterate over
 *  @param iterator The user provided function to call for each node.
 *                  Returns true to delete the node, false to leave the
 *                  node unaltered by the iteration.  If the value of NULL,
 *                  is passed in each node in the list is deleted.
 *  @param deleter the user provided function called to delete the node
 */
void rebar_ll_iterate( rebar_ll_list_t *list,
                      rebar_ll_iterator_fn_t iterator,
                      rebar_ll_delete_node_fn_t deleter,
                      void *user_data );

/**
 *  Used to iterate a list starting at a specific node in the
 *  linked list.  This API is not intended to delete nodes. It's
 *  more of an extension to rebar_ll_find.
 *
 *  @param node from which to start the iteration 
 *  @param iterator The user provided function to call for each
 *                  node. It returns conitinue or stop. 
 *  @param user_data data passed to the iterator 

 */
void rebar_ll_iterate_from (rebar_ll_node_t *node,
                           rebar_ll_iterator_fn_t iterator,
                           void *user_data);
/**
 *  Used to delete all nodes in a list.  For each node in the list,
 *  the user provided deleter function is called and must free any
 *  appropriate memory.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @note No memory is released in this operation.  It is up to
 *  the caller of this function to make sure they have a copy
 *  of the head node pointer to prevent memory leaks.
 *
 *  @param list the list to delete the nodes of
 *  @param deleter the user provided function called to delete the node
 */
#define rebar_ll_delete_all( list, deleter, user_data ) \
    rebar_ll_iterate( list, NULL, deleter, user_data )

/**
 *  Used to count the number of nodes in a list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *
 *  @param list the list to count the nodes for
 *
 *  @return the number of items in the list
 */
size_t rebar_ll_count( rebar_ll_list_t *list );

/**
 *  Used to find a specific node in the linked list.
 *
 *  @note Do not pass in NULL for the list or it will be dereferenced!
 *  @note Do not pass in NULL for the cmp_fn or it will be dereferenced!
 *
 *  @note See rebar_ll_get_data() for details about struct_name and node_name
 *        useage.
 *
 *  @param list the list to search through
 *  @param cmp_fn the comparison function to use when comparing 2 nodes
 *  @param needle the data the list is being checked for
 *  @param struct_name the user structure name
 *  @param node_name the name of the linked list node name
 *
 *  @return the node that matches the needle's data if one is found,
 *          NULL otherwise
 */
#define rebar_ll_find( list, cmp_fn, needle, struct_name, node_name ) \
    __rebar_ll_find( list, cmp_fn, needle, offsetof(struct_name, node_name) )
rebar_ll_node_t *__rebar_ll_find( rebar_ll_list_t *list,
                                rebar_ll_cmp_node_fn_t cmp_fn,
                                void *needle,
                                int offset );

/*----------------------------------------------------------------------------*/
/*                             Doubly Linked List                             */
/*----------------------------------------------------------------------------*/

/* Not currently implemented. */

#ifdef __cplusplus
}
#endif

#endif
