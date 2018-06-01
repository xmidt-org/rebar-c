#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "../src/rebar-c.h"
#include "general.h"
#include "test_hashmap.h"
#include "test_queue.h"


struct _foo1 {
    rebar_ll_node_t my_node;
    int data;
    int bar;
    char cdata[5];
};

struct _foo2 {
    int data;
    int bar;
    rebar_ll_node_t my_node;
    char cdata[5];
};

struct _foo3 {
    int data;
    int bar;
    char cdata[5];
    rebar_ll_node_t my_node;
};

void test_list_init( void )
{
    rebar_ll_list_t list;

    CU_PASS( "list init of NULL passed" );

    memset( &list, 55, sizeof(rebar_ll_list_t) );
    CU_ASSERT_PTR_NOT_NULL( list.head );
    CU_ASSERT_PTR_NOT_NULL( list.tail );

    rebar_ll_init( &list );

    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );
    CU_PASS( "list init of valid list passed" );
}

rebar_ll_node_t * create_test_node( void )
{
    rebar_ll_node_t * node = malloc( sizeof(rebar_ll_node_t) );
    CU_ASSERT_FATAL(NULL != node);
    memset( node, 55, sizeof(rebar_ll_node_t) );
    return node;
}

void delete_test_node( rebar_ll_node_t *node )
{
    free( (void*)node );
}

void test_list_append( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();

    CU_ASSERT_PTR_NOT_NULL( node1->next );
    CU_ASSERT_PTR_NOT_NULL( node2->next );
    CU_ASSERT_PTR_NOT_NULL( node3->next );

    rebar_ll_init( &list );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_append( &list, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_append( &list, node1 );

    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    rebar_ll_append( &list, node2 );

    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node2 );
    CU_ASSERT_PTR_EQUAL( node1->next, node2 );
    CU_ASSERT_PTR_NULL( node2->next );

    rebar_ll_append( &list, node3 );

    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( node1->next, node2 );
    CU_ASSERT_PTR_EQUAL( node2->next, node3 );
    CU_ASSERT_PTR_NULL( node3->next );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
}

void test_get_first( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *ptr;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();

    rebar_ll_init( &list );

    ptr = rebar_ll_get_first( &list );
    CU_ASSERT_PTR_NULL( ptr );

    rebar_ll_append( &list, node1 );
    ptr = rebar_ll_get_first( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node1 );

    rebar_ll_append( &list, node2 );
    ptr = rebar_ll_get_first( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node1 );

    rebar_ll_append( &list, node3 );
    ptr = rebar_ll_get_first( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node1 );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
}

void test_get_next( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *ptr;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();

    ptr = rebar_ll_get_next( (rebar_ll_node_t*) NULL );
    CU_ASSERT_PTR_NULL( ptr );

    rebar_ll_init( &list );
    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );

    ptr = rebar_ll_get_first( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node1 );
    ptr = rebar_ll_get_next( ptr );
    CU_ASSERT_PTR_EQUAL( ptr, node2 );
    ptr = rebar_ll_get_next( ptr );
    CU_ASSERT_PTR_EQUAL( ptr, node3 );
    ptr = rebar_ll_get_next( ptr );
    CU_ASSERT_PTR_NULL( ptr );
    ptr = rebar_ll_get_next( ptr );
    CU_ASSERT_PTR_NULL( ptr );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
}

void test_get_last( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *ptr;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();

    rebar_ll_init( &list );

    ptr = rebar_ll_get_last( &list );
    CU_ASSERT_PTR_NULL( ptr );

    rebar_ll_append( &list, node1 );
    ptr = rebar_ll_get_last( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node1 );

    rebar_ll_append( &list, node2 );
    ptr = rebar_ll_get_last( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node2 );

    rebar_ll_append( &list, node3 );
    ptr = rebar_ll_get_last( &list );
    CU_ASSERT_PTR_EQUAL( ptr, node3 );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
}

void test_list_prepend( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();

    CU_ASSERT_PTR_NOT_NULL( node1->next );
    CU_ASSERT_PTR_NOT_NULL( node2->next );
    CU_ASSERT_PTR_NOT_NULL( node3->next );

    rebar_ll_init( &list );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_prepend( &list, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_prepend( &list, node1 );

    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    rebar_ll_prepend( &list, node2 );

    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_EQUAL( node2->next, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    rebar_ll_prepend( &list, node3 );

    CU_ASSERT_PTR_EQUAL( list.head, node3 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_EQUAL( node3->next, node2 );
    CU_ASSERT_PTR_EQUAL( node2->next, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
}

void test_list_insert( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();
    rebar_ll_node_t *node4 = create_test_node();
    rebar_ll_node_t *node5 = create_test_node();
    rebar_ll_node_t *node6 = create_test_node();

    rebar_ll_init( &list );
    rebar_ll_insert( &list, NULL, NULL, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_insert( &list, node1, NULL, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    /* No nodes in the list, but choose a random one as the insertion point. */
    rebar_ll_insert( &list, node1, node2, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    /* List: <empty>
     */
    rebar_ll_prepend( &list, node1 );
    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    /* List: node1 -> node2, node1
     */
    rebar_ll_insert( &list, node2, node1, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );
    CU_ASSERT_PTR_EQUAL( node2->next, node1 );
    CU_ASSERT_PTR_NULL( node1->next );

    /* List: node2, node1 -> node2, node1, node3
     */
    rebar_ll_insert( &list, node3, node1, REBAR_MODE__AFTER );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( node2->next, node1 );
    CU_ASSERT_PTR_EQUAL( node1->next, node3 );
    CU_ASSERT_PTR_NULL( node3->next );

    /* List: node2, node1, node3 -> node2, node4, node1, node3
     */
    rebar_ll_insert( &list, node4, node2, REBAR_MODE__AFTER );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( node2->next, node4 );
    CU_ASSERT_PTR_EQUAL( node4->next, node1 );
    CU_ASSERT_PTR_EQUAL( node1->next, node3 );
    CU_ASSERT_PTR_NULL( node3->next );

    /* List: node2, node4, node1, node3 -> node2, node4, node1, node5, node3
     */
    rebar_ll_insert( &list, node5, node3, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( node2->next, node4 );
    CU_ASSERT_PTR_EQUAL( node4->next, node1 );
    CU_ASSERT_PTR_EQUAL( node1->next, node5 );
    CU_ASSERT_PTR_EQUAL( node5->next, node3 );
    CU_ASSERT_PTR_NULL( node3->next );

    /* List: node2, node4, node1, node5, node3 -> node2, node4, node1, node5, node3
     * Invalid node as the insertion point.
     */
    rebar_ll_insert( &list, node6, node6, REBAR_MODE__BEFORE );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( node2->next, node4 );
    CU_ASSERT_PTR_EQUAL( node4->next, node1 );
    CU_ASSERT_PTR_EQUAL( node1->next, node5 );
    CU_ASSERT_PTR_EQUAL( node5->next, node3 );
    CU_ASSERT_PTR_NULL( node3->next );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
    delete_test_node(node4);
    delete_test_node(node5);
    delete_test_node(node6);
}

void test_list_remove_head( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();

    rebar_ll_init( &list );
    rebar_ll_remove_head( &list );
    CU_PASS( "list remove_head of empty list passed" );


    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );

    rebar_ll_remove_head( &list );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node2 );
    CU_ASSERT_PTR_NULL( node2->next );

    rebar_ll_remove_head( &list );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    delete_test_node(node1);
    delete_test_node(node2);
}

rebar_ll_iterator_response_t iterate1( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)

    CU_FAIL( "iterate1 should not be called\n" );
    return REBAR_IR__CONTINUE;
}

rebar_ll_iterator_response_t iterate2( rebar_ll_node_t *node, void *user_data )
{
    CU_ASSERT_PTR_NULL( user_data );
    CU_ASSERT_PTR_NULL( node->next );
    CU_PASS( "iterate2 called.\n" );
    return REBAR_IR__CONTINUE;
}

static int iterate3_call_count = 0;
rebar_ll_iterator_response_t iterate3( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)

    switch( iterate3_call_count++ ) {
        case 0:
            return REBAR_IR__CONTINUE;
        case 1:
            return REBAR_IR__DELETE_AND_CONTINUE;
        case 2:
            return REBAR_IR__DELETE_AND_CONTINUE;
        case 3:
            return REBAR_IR__CONTINUE;
        default:
            break;
    }

    return REBAR_IR__CONTINUE;
}

static int iterate4_call_count = 0;
rebar_ll_iterator_response_t iterate4( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)
	
    switch( iterate4_call_count++ ) {
        case 0:
            return REBAR_IR__DELETE_AND_CONTINUE;
        case 1:
            return REBAR_IR__CONTINUE;
        case 2:
            return REBAR_IR__CONTINUE;
        case 3:
            return REBAR_IR__DELETE_AND_CONTINUE;
        default:
            break;
    }

    return REBAR_IR__CONTINUE;
}

static int iterate5_call_count = 0;
rebar_ll_iterator_response_t iterate5( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)
	
    switch( iterate5_call_count++ ) {
        case 0:
            return REBAR_IR__STOP;
        default:
            break;
    }

    return REBAR_IR__CONTINUE;
}

static int iterate6_call_count = 0;
rebar_ll_iterator_response_t iterate6( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)
	
    switch( iterate6_call_count++ ) {
        case 0:
            return REBAR_IR__DELETE_AND_CONTINUE;
        case 1:
            return REBAR_IR__DELETE_AND_STOP;
        default:
            break;
    }

    return REBAR_IR__DELETE_AND_CONTINUE;
}
static int iterate7_call_count = 0;
rebar_ll_iterator_response_t iterate7( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)

    iterate7_call_count++;
    return REBAR_IR__CONTINUE;
}


void destroy1( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(node)
    IGNORE_UNUSED(user_data)

    CU_FAIL( "destroy1 should not be called\n" );
}

static int destroy2_call_count = 0;
void destroy2( rebar_ll_node_t *node, void *user_data )
{
    IGNORE_UNUSED(user_data)
	
    destroy2_call_count++;
    delete_test_node(node);
}

void test_list_iterate( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();
    rebar_ll_node_t *node4 = create_test_node();

    rebar_ll_init( &list );
    rebar_ll_iterate( &list, NULL, NULL, NULL );
    rebar_ll_iterate( &list, iterate1, NULL, NULL );
    rebar_ll_iterate( &list, iterate1, destroy1, NULL );

    rebar_ll_append( &list, node1 );
    rebar_ll_iterate( &list, NULL, NULL, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    rebar_ll_append( &list, node1 );
    rebar_ll_iterate( &list, iterate2, NULL, NULL );
    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node1 );

    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    /* List: node1, node2, node3, node4 -> node1, node4 */
    rebar_ll_iterate( &list, iterate3, destroy2, NULL );
    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node4 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node4 );
    CU_ASSERT( 2 == destroy2_call_count );
    CU_ASSERT( 4 == iterate3_call_count );

    /* Clear the list */
    rebar_ll_iterate( &list, NULL, NULL, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    node2 = create_test_node();
    node3 = create_test_node();

    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    /* List: node1, node2, node3, node4 -> node2, node3 */
    destroy2_call_count = 0;
    rebar_ll_iterate( &list, iterate4, destroy2, NULL );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node3 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node3 );
    CU_ASSERT( 2 == destroy2_call_count );
    CU_ASSERT( 4 == iterate3_call_count );

    /* Clear the list */
    rebar_ll_iterate( &list, NULL, NULL, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );

    node1 = create_test_node();
    node4 = create_test_node();

    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    /* List: node1, node2, node3, node4 -> (unchanged) */
    rebar_ll_iterate( &list, iterate5, NULL, NULL );
    CU_ASSERT( 1 == iterate5_call_count );

    /* List: node1, node2, node3, node4 -> node3, node4 */
    rebar_ll_iterate( &list, iterate6, NULL, NULL );
    CU_ASSERT( 2 == iterate6_call_count );
    CU_ASSERT_PTR_EQUAL( list.head, node3 );
    CU_ASSERT_PTR_EQUAL( list.tail, node4 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node4 );

    rebar_ll_init( &list );
    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    rebar_ll_iterate_from( node2, iterate7, NULL );
    CU_ASSERT( 2 == iterate7_call_count );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
    delete_test_node(node4);
}

void test_list_delete_all( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();
    rebar_ll_node_t *node4 = create_test_node();

    rebar_ll_init( &list );
    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    destroy2_call_count = 0;
    rebar_ll_delete_all( &list, destroy2, NULL );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );
    CU_ASSERT( 4 == destroy2_call_count );
}

void test_list_remove( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();
    rebar_ll_node_t *node4 = create_test_node();

    rebar_ll_init( &list );
    rebar_ll_append( &list, node1 );
    rebar_ll_append( &list, node2 );
    rebar_ll_append( &list, node3 );
    rebar_ll_append( &list, node4 );

    /* List: node1, node2, node3, node4 -> (unchanged) */
    rebar_ll_remove( &list, NULL );
    CU_ASSERT_PTR_EQUAL( list.head, node1 );
    CU_ASSERT_PTR_EQUAL( list.tail, node4 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node2 );
    CU_ASSERT_PTR_EQUAL( list.head->next->next, node3 );
    CU_ASSERT_PTR_EQUAL( list.head->next->next->next, node4 );
    CU_ASSERT_PTR_NULL ( list.head->next->next->next->next );

    /* List: node1, node2, node3, node4 -> node2, node3, node4 */
    rebar_ll_remove( &list, node1 );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node4 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node3 );
    CU_ASSERT_PTR_EQUAL( list.head->next->next, node4 );
    CU_ASSERT_PTR_NULL ( list.head->next->next->next );

    /* List: node2, node3, node4 -> node2, node4 */
    rebar_ll_remove( &list, node3 );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node4 );
    CU_ASSERT_PTR_EQUAL( list.head->next, node4 );
    CU_ASSERT_PTR_NULL ( list.head->next->next );

    /* List: node2, node4 -> node2 */
    rebar_ll_remove( &list, node4 );
    CU_ASSERT_PTR_EQUAL( list.head, node2 );
    CU_ASSERT_PTR_EQUAL( list.tail, node2 );
    CU_ASSERT_PTR_NULL ( list.head->next );

    /* List: node2, -> (empty) */
    rebar_ll_remove( &list, node2 );
    CU_ASSERT_PTR_NULL( list.head );
    CU_ASSERT_PTR_NULL( list.tail );


    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
    delete_test_node(node4);
}

void test_list_get_count( void )
{
    rebar_ll_list_t list;
    rebar_ll_node_t *node1 = create_test_node();
    rebar_ll_node_t *node2 = create_test_node();
    rebar_ll_node_t *node3 = create_test_node();
    rebar_ll_node_t *node4 = create_test_node();

    rebar_ll_init( &list );
    CU_ASSERT( 0 == rebar_ll_count(&list) );
    rebar_ll_append( &list, node1 );
    CU_ASSERT( 1 == rebar_ll_count(&list) );
    rebar_ll_append( &list, node2 );
    CU_ASSERT( 2 == rebar_ll_count(&list) );
    rebar_ll_append( &list, node3 );
    CU_ASSERT( 3 == rebar_ll_count(&list) );
    rebar_ll_append( &list, node4 );
    CU_ASSERT( 4 == rebar_ll_count(&list) );

    rebar_ll_delete_all( &list, NULL, NULL );
    CU_ASSERT( 0 == rebar_ll_count(&list) );

    delete_test_node(node1);
    delete_test_node(node2);
    delete_test_node(node3);
    delete_test_node(node4);
}

void test_list_get_data( void )
{
    rebar_ll_node_t *node;

    struct _foo1 foo1;
    struct _foo2 foo2;
    struct _foo3 foo3;

    node = &foo1.my_node;
    CU_ASSERT_PTR_EQUAL( &foo1, rebar_ll_get_data(struct _foo1, my_node, node) );
    CU_ASSERT_PTR_EQUAL( node, &(rebar_ll_get_data(struct _foo1, my_node, node))->my_node );
    //printf( "node: %p, foo1: %p, converted: %p\n", node, &foo1, rebar_ll_get_data(struct _foo1, my_node, node) );

    node = &foo2.my_node;
    CU_ASSERT_PTR_EQUAL( &foo2, rebar_ll_get_data(struct _foo2, my_node, node) );
    CU_ASSERT_PTR_EQUAL( node, &(rebar_ll_get_data(struct _foo2, my_node, node))->my_node );
    //printf( "node: %p, foo2: %p, converted: %p\n", node, &foo2, rebar_ll_get_data(struct _foo2, my_node, node) );

    node = &foo3.my_node;
    CU_ASSERT_PTR_EQUAL( &foo3, rebar_ll_get_data(struct _foo3, my_node, node) );
    CU_ASSERT_PTR_EQUAL( node, &(rebar_ll_get_data(struct _foo3, my_node, node))->my_node );
    //printf( "node: %p, foo3: %p, converted: %p\n", node, &foo3, rebar_ll_get_data(struct _foo3, my_node, node) );
}

int list_cmp1( void *node_1, void *node_2 )
{
    struct _foo1 *a = (struct _foo1*) node_1;
    struct _foo1 *b = (struct _foo1*) node_2;

    return a->data - b->data;
}

int list_cmp2( void *node_1, void *node_2 )
{
    struct _foo2 *a = (struct _foo2*) node_1;
    struct _foo2 *b = (struct _foo2*) node_2;

    return a->data - b->data;
}

int list_cmp3( void *node_1, void *node_2 )
{
    struct _foo3 *a = (struct _foo3*) node_1;
    struct _foo3 *b = (struct _foo3*) node_2;

    return a->data - b->data;
}


void test_list_find( void )
{
    int i;
    rebar_ll_list_t list;

    struct _foo1 foo1[4];
    struct _foo2 foo2[4];
    struct _foo3 foo3[4];

    for( i = 0; i < 4; i++ ) {
        foo1[i].data = i;
        foo1[i].bar = 0;
        foo1[i].cdata[0] = 0;
        foo1[i].cdata[1] = 1;
        foo1[i].cdata[2] = 2;
        foo1[i].cdata[3] = 3;
        foo1[i].cdata[4] = 4;

        foo2[i].data = i;
        foo2[i].bar = 0;
        foo2[i].cdata[0] = 0;
        foo2[i].cdata[1] = 1;
        foo2[i].cdata[2] = 2;
        foo2[i].cdata[3] = 3;
        foo2[i].cdata[4] = 4;

        foo3[i].data = i;
        foo3[i].bar = 0;
        foo3[i].cdata[0] = 0;
        foo3[i].cdata[1] = 1;
        foo3[i].cdata[2] = 2;
        foo3[i].cdata[3] = 3;
        foo3[i].cdata[4] = 4;
    }

    rebar_ll_init( &list );
    rebar_ll_append( &list, &foo1[0].my_node );
    rebar_ll_append( &list, &foo1[1].my_node );
    rebar_ll_append( &list, &foo1[2].my_node );

    CU_ASSERT( &foo1[2].my_node == rebar_ll_find(&list, list_cmp1, &foo1[2],
               struct _foo1, my_node) );
    CU_ASSERT( NULL == rebar_ll_find(&list, list_cmp1, &foo1[3],
               struct _foo1, my_node) );

    rebar_ll_init( &list );
    rebar_ll_append( &list, &foo2[0].my_node );
    rebar_ll_append( &list, &foo2[1].my_node );
    rebar_ll_append( &list, &foo2[2].my_node );

    CU_ASSERT( &foo2[2].my_node == rebar_ll_find(&list, list_cmp2, &foo2[2],
               struct _foo2, my_node) );
    CU_ASSERT( NULL == rebar_ll_find(&list, list_cmp2, &foo2[3],
               struct _foo2, my_node) );

    rebar_ll_init( &list );
    rebar_ll_append( &list, &foo3[0].my_node );
    rebar_ll_append( &list, &foo3[1].my_node );
    rebar_ll_append( &list, &foo3[2].my_node );

    CU_ASSERT( &foo3[2].my_node == rebar_ll_find(&list, list_cmp3, &foo3[2],
               struct _foo3, my_node) );
    CU_ASSERT( NULL == rebar_ll_find(&list, list_cmp3, &foo3[3],
               struct _foo3, my_node) );
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "Singly Linked List Test", NULL, NULL );
    CU_add_test( *suite, "Test rebar_ll_init()       ", test_list_init );
    CU_add_test( *suite, "Test rebar_ll_append()     ", test_list_append );
    CU_add_test( *suite, "Test rebar_ll_get_first()  ", test_get_first );
    CU_add_test( *suite, "Test rebar_ll_get_next()   ", test_get_next );
    CU_add_test( *suite, "Test rebar_ll_get_last()   ", test_get_last );
    CU_add_test( *suite, "Test rebar_ll_prepend()    ", test_list_prepend );
    CU_add_test( *suite, "Test rebar_ll_insert()     ", test_list_insert );
    CU_add_test( *suite, "Test rebar_ll_remove_head()", test_list_remove_head );
    CU_add_test( *suite, "Test rebar_ll_iterate()    ", test_list_iterate );
    CU_add_test( *suite, "Test rebar_ll_delete_all() ", test_list_delete_all );
    CU_add_test( *suite, "Test rebar_ll_remove()     ", test_list_remove );
    CU_add_test( *suite, "Test rebar_ll_count()      ", test_list_get_count );
    CU_add_test( *suite, "Test rebar_ll_find()       ", test_list_find );
    CU_add_test( *suite, "Test rebar_ll_get_data()     ", test_list_get_data );
    /* Start Tests for HASHMAP */
    add_hashmap_tests(suite);
    /* Start test of Queue APIs */
    add_queue_tests(suite);
    
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( void )
{
    unsigned rv = 1;
    CU_pSuite suite;

    if( CUE_SUCCESS == CU_initialize_registry() ) 
    {
        add_suites( &suite );

        if( NULL != suite ) 
        {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
        
            CU_basic_show_failures( CU_get_failure_list() );
           
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();
    }

    if( 0 != rv ) 
    {
        return 1;
    }
    return 0;
}


