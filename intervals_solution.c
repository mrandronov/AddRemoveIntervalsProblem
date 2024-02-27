/*

Problem Statement:

We are looking for a program that manages disjointed intervals of integers. 

E.g.: {[1, 3), [4, 6)} is a valid object that contains two intervals. 
{[1, 3), [3, 6)} is not a valid object because it is not disjoint. {[1, 6)} is the intended result.

Empty set {} means no interval, it is the default/start state.
 
We want you to implement two functions:
 
add(newLeft, newRight)
remove(newLeft, newRight)
 
Here is an example sequence:
 
Start: {}
Call: add(1, 5)  =>  {[1, 5)}
Call: remove(2, 3) => {[1, 2), [3, 5)}
Call: add(6, 8) => {[1, 2), [3, 5), [6, 8)}
Call: remove(4, 7) => {[1, 2), [3, 4), [7, 8)}
Call: add(2, 7) => {[1, 8)}
 
etc.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHOULD_PRINT 1
#define SHOULD_NOT_PRINT 0

typedef struct INTERVAL
{
        int                     left;
        int                     right;
        struct INTERVAL*        next;
        struct INTERVAL*        prev;
} interval_t;

typedef struct
{
        interval_t*     head;
        interval_t*     tail;
} interval_set_t;

void
interval_print( interval_t* i )
{
        printf( "[%d, %d)\n", i->left, i->right );
}

void
interval_set_print( interval_set_t* is )
{
        interval_t*     p = is->head;

        printf( "{" );

        while ( p )
        {
                printf( "[%d, %d)", p->left, p->right );

                if ( p->next )
                {
                        printf( ", " );
                }

                p = p->next;
        }
        printf( "}\n" );
}

void
interval_set_free( interval_set_t* is )
{
        interval_t*     prev;
        interval_t*     p = is->head;

        while (p)
        {
                prev = p;
                p = p->next;
                free(prev);
        }

        free(is);
}

void
interval_set_add( interval_set_t* is,
                int newLeft,
                int newRight,
                int should_print )
{
        if ( newLeft >= newRight )
        {
                return;
        }

        if ( !is->head )
        {
                is->head = ( interval_t* ) calloc( 1, sizeof( interval_t ) );

                is->head->left = newLeft;
                is->head->right = newRight;
                is->tail = is->head;
        }

        if ( newRight < is->head->left )
        {
                /*
                        Both 'newLeft' and 'newRight' represent a range LESS
                        than any existing interval.
                 */

                interval_t*             temp = ( interval_t* ) calloc( 1, sizeof( interval_t ) );
                temp->left = newLeft;
                temp->right = newRight;

                temp->next = is->head;
                is->head->prev = temp;
                is->head = temp;
        }
        else if ( newLeft > is->tail->right )
        {
                /*
                        Both 'newLeft' and 'newRight' represent a range GREATER
                        than any existing interval.
                 */

                interval_t*             temp = ( interval_t* ) calloc( 1, sizeof( interval_t ) );
                temp->left = newLeft;
                temp->right = newRight;

                is->tail->next = temp;
                temp->prev = is->tail;
                is->tail = temp;
        }

        /*
                In the following while loop, we look for the interval nodes
                that 'newLeft' and 'newRight' are within the range of ( pointed 
                to by lNode and rNode respectively ). 

                The value of the pointers is then used to determine how the 
                range [newLeft, newRight) can be added to the interval set.
         */

        interval_t*     p = is->head;
        interval_t*     lNode = NULL;
        interval_t*     rNode = NULL;

        while ( p )
        {
                if ( p->left <= newLeft && newLeft <= p->right )
                {
                        lNode = p;
                }

                if ( p->left <= newRight && newRight <= p->right )
                {
                        rNode = p;
                }

                p = p->next;
        }

        if ( ( lNode && rNode ) && ( lNode != rNode ) )
        {
                /*
                        If lNode and rNode are both not null AND not 
                        identical, then 'newLeft' and 'newRight' are within 
                        intervals that are already in the set and we
                        can simply update the nodes and delete everything
                        inbetween.

                        For example:
                                Given :{ [1, 2), [5, 7), [10, 14) } 
                                then Add: [2, 11)
                                The result would be: { [1, 3), [10, 14) }

                        It is important for them to not be same, because
                        if they were identical, then that means the range
                        [newLeft, newRight) already exists in a range in the set.

                        For example:
                                Given: { [1, 7), [10, 14) }
                                then Add: [2, 5)
                                The result would be: { [1, 7), [10, 14) }
                                ( Notice that it doesn't change! )

                        This would essentially be a no-op and as such is not
                        checked by any if or else-if statement.
                 */

                interval_t*             temp = lNode->next;
 
                lNode->right = rNode->right;
                lNode->next = rNode->next;
 
                while ( temp != lNode->next )
                {
                        temp = temp->next;
                        free( temp->prev );
                }

                if ( !lNode->next )
                {
                        is->tail = lNode;
                }
        }
        else if ( !lNode && rNode )
        {
                /*
                        If lNode is not defined and rNode is defined, then we
                        know 'newLeft' is less than any interval in the set and 
                        'newRight' is within an interval in the set.

                        Therefore, we can update the node 'newRight' would belong in 
                        and delete all nodes before it.
                 */

                interval_t*             temp = is->head;

                rNode->left = newLeft;

                while ( temp != rNode )
                {
                        temp = temp->next;
                        free( temp->prev );
                }
 
                is->head = rNode;
        }
        else if ( lNode && !rNode )
        {
                /*
                        If lNode is defined and rNode is not defined, then
                        'newLeft' is within an interval in the set AND 'newRight' is 
                        greater than any interval in the set.

                        This is the opposite case of the previous else-if
                        condition.

                        We can update the node 'newLeft' belongs in and delete all 
                        nodes after it.
                 */

                interval_t*     temp = lNode->next;

                lNode->right = newRight;
                is->tail = lNode;

                while ( temp )
                {
                        temp = temp->next;
                        free( temp->prev );
                }

                lNode->next = NULL;
        }
        else if ( !lNode && !rNode )
        {
                /*
                        If neither lNode or rNode are defined at this point,
                        then there are 4 subcases we need newRight check newRight add the
                        interval [newLeft, newRight).
                 */

                if ( newLeft < is->head->left && newRight > is->tail->right )
                {
                        /*
                                Case 1: both 'newLeft' and 'newRight' represent a range 
                                that begins before any range in the set and ends after
                                any range in the set.

                                Since this consumes the entire existing set, we can just 
                                replace the set that was passed as an argument with a 
                                new one solely containing the range [newLeft, newRight).
                         */

                        interval_set_free( is );

                        interval_set_t*         new_is = ( interval_set_t * ) calloc( 1, sizeof( interval_set_t ) );

                        interval_set_add(new_is, newLeft, newRight, SHOULD_NOT_PRINT);

                        is = new_is;
                }
                else if ( newLeft > is->head->left && newRight < is->tail->right )
                {
                        /*
                                Case 2: both 'newLeft' and 'newRight' represent a range in
                                between existing intervals in the set.

                                This could be either a case where both 'newLeft' and
                                'newRight' are between any interval in the set or the case
                                where 'newLeft' and 'newRight' is a range that covers multiple
                                nodes in the set, in which case those nodes would get deleted
                                and a new node inserted in the set.
                         */

                        interval_t*             p = is->head;
                        interval_t*             nearestLeftNode = NULL;
                        interval_t*             nearestRightNode = NULL;

                        while ( p )
                        {
                                if ( newLeft < p->left && !nearestLeftNode )
                                {
                                        nearestLeftNode = p;
                                }
                                if ( newRight < p->left && !nearestRightNode )
                                {
                                        nearestRightNode = p;
                                }

                                if ( nearestLeftNode && nearestRightNode )
                                {
                                        break;
                                }

                                p = p->next;
                        }

                        /*
                                First insert the newNode before the nearestLeftNode.
                         */

                        interval_t*             newNode = ( interval_t* ) calloc( 1, sizeof( interval_t ) );

                        newNode->left = newLeft;
                        newNode->right = newRight;

                        nearestLeftNode->prev->next = newNode;

                        /*
                                newNode should point to nearestRightNode, since we will
                                free all nodes from nearestLeftNode ( inclusively ) to 
                                nearestRightNode ( exclusively ).
                         */

                        newNode->next = nearestRightNode;
                        
                        while ( nearestLeftNode != nearestRightNode )
                        {
                                nearestLeftNode = nearestLeftNode->next;
                                free( nearestLeftNode->prev );
                        }
                }
                else if ( newLeft < is->head->left && newRight > is->head->left )
                {
                        /*
                                Case 3: 'newLeft' and 'newRight' represent a range that starts 
                                less than any existing range and ends somewhere between existing 
                                intervals in the set.
                         */

                        interval_t*             temp = is->head;

                        rNode->right = newRight;
                        rNode->left = newLeft;
                        
                        while ( temp != rNode )
                        {
                                temp = temp->next;
                                free( temp->prev );
                        }
                        
                        is->head = rNode;
                }
                else if ( newLeft > is->head->left && newRight > is->tail->right )
                {
                        /*
                                Case 4: 'newLeft' and 'newRight' represent a range that 
                                starts between existing ranges in the set and ends 
                                greater than any existing range.
                         */

                        interval_t*             temp = lNode->next;

                        lNode->right = newRight;
                        lNode->left = newLeft;

                        is->tail = lNode;

                        while ( temp )
                        {
                                temp = temp->next;
                                free( temp->prev );
                        }

                        lNode->next = NULL;
                }
        }
        
        if ( should_print )
        {
                interval_set_print( is );
        }
}

void
interval_set_remove( interval_set_t* is,
                int newLeft,
                int newRight,
                int should_print )
{
        if ( !is->head || newLeft >= newRight )
        {
                return;
        }
        
        interval_t*             p = is->head;
        interval_t*             lNode = NULL;
        interval_t*             rNode = NULL;

        /*
                In the following while loop, we look for the interval nodes
                that newLeft and newRight are within the range of ( pointed
                to by lNode and rNode respectively). The value of the 
                pointers is then used to determine how the range 
                [newLeft, newRight) can be excluded from the interval set.
         */

        while ( p )
        {
                if ( p->left <= newLeft && newLeft <= p->right )
                {
                        lNode = p;
                }

                if ( p->left <= newRight && newRight <= p->right )
                {
                        rNode = p;
                }

                p = p->next;
        }

        if ( ( lNode && rNode ) && ( lNode != rNode ) )
        {
                /*
                        When lNode and rNode are defined and are not equal to 
                        eachother, this means 'newLeft' and 'newRight' are values 
                        that exist in two different intervals in the set.

                        We can then update those two nodes accordingly and
                        delete all existing nodes inbetween.
                 */

                interval_t*             limit;
                interval_t*             temp = lNode->next;
                
                lNode->right = newLeft;

                if ( rNode->right == newRight )
                {
                        limit = rNode->next;
                }
                else
                {
                        rNode->left = newRight;
                        limit = rNode;
                }

                lNode->next = limit;
                
                while ( temp != limit )
                {
                        temp = temp->next;
                        free( temp->prev );
                }
                
                if ( !lNode->next )
                {
                        is->tail = lNode;
                }
        }
        else if ( !lNode && rNode )
        {
                /*
                        When lNode is not defined and rNode is, this means there
                        are 2 possible cases:

                        - Either 'newLeft' exists below all the intervals in the 
                        set and 'newRight' exists WITHIN an interval in the set.

                        - Or 'newleft' exists in between some intervals in the
                        set and 'newRight' exists WITHIN an interval in the set.

                        For the first case, we can update the node 'newRight' 
                        exists in accordingly and remove all nodes before it.

                        For the second case, this is an invalid operation, since
                        we can't be certain would the new left value for any
                        interval would be. We can do nothing and exit gracefully 
                        in this case.
                 */

                if ( newLeft < is->head->left )
                {
                        interval_t*     temp = is->head;

                        rNode->left = newRight;

                        while ( temp != rNode )
                        {
                                temp = temp->next;
                                free( temp->prev );
                        }
                        
                        is->head = rNode;
                }
        }
        else if ( lNode && !rNode )
        {
                /*
                        This is the same as the previous else-if case except
                        reversed. There are still 2 cases:

                        - Either 'newLeft' exists below all the intervals in the 
                        set and 'newRight' exists WITHIN an interval in the set.

                        - Or 'newleft' exists in between some intervals in the
                        set and 'newRight' exists WITHIN an interval in the set.

                        Again for the first case, we can update the node 
                        'newLeft' exists in accordingly and remove all nodes 
                        following it.

                        For the second case, this would also be a no-op and can
                        exit gracefully.
                 */

                if ( newRight > is->tail->right )
                {
                        interval_t*     temp = lNode->next;

                        lNode->right = newLeft;
                        is->tail = lNode;
                        
                        while ( temp )
                        {
                                temp = temp->next;
                                free( temp->prev );
                        }

                        lNode->next = NULL;
                }
        }
        else if ( !lNode && !rNode )
        {
                /*
                        If neither lNode or rNode is defined, then the range 
                        [newLeft, newRight) simply does not exist anywhere in the 
                        interval set and no work is necessary with one exception.

                        The only case we need to check here is if the range 
                        [newLeft, newRight) covers the entire range. In which 
                        case, the entire set needs to be effectively deleted. 

                        Therefore, we can just free the interval set passed as
                        an argument and replace it with a newly allocated one.
                 */

                if ( newLeft < is->head->left && newRight > is->tail->right )
                {
                        interval_set_free(is);

                        interval_set_t*         new_is = ( interval_set_t* ) calloc( 1, sizeof( interval_set_t ) );
                        
                        is = new_is;
                }
        }
        else if ( lNode == rNode )
        {
                /*
                        If lNode and rNode are defined (which was checked by
                        a previous if condition) and they are identical, then
                        this means that the interval we are removing is inside 
                        of an interval in the set. 

                        In this case, there are four possible subcases that 
                        need we need to check for.
                 */

                if ( lNode->left == newLeft && rNode->right == newRight )
                {
                        /*
                                Case 1: [newLeft, newRight) covers the entire range of the 
                                interval node, in which case we just need to delete it.
                         */

                        interval_t*     next = lNode->next;
                        interval_t*     prev = lNode->prev;
                        
                        if (prev)
                        {
                                prev->next = next;
                        }
                        else
                        {
                                is->head = next;
                        }
                        
                        if (next)
                        {
                                next->prev = prev;
                        }
                        else
                        {
                                is->tail = prev;
                        }

                        free(lNode);
                }
                else if ( lNode->left == newLeft && newRight <= lNode->right )
                {
                        /*
                                Case 2: [newLeft, newRight) starts at the node's left 
                                value and ends somewhere inside the interval range.
                         */

                        lNode->left = newRight;
                }
                else if ( lNode->left <= newLeft && newRight == lNode->right )
                {
                        /*
                                Case 3: [newLeft, newRight) starts somwhere inside the 
                                interval range and ends at the node's right value.
                         */

                        lNode->right = newLeft;
                }
                else if ( lNode->left < newLeft && newRight < lNode->right )
                {
                        /*
                                Case 4: [newLeft, newRight) partially covers the range 
                                of the interval node, in which case it must be
                                split in to two nodes.
                         */

                        interval_t*     newNode = ( interval_t* ) calloc( 1, sizeof( interval_t ) );

                        newNode->left = newRight;
                        newNode->right = lNode->right;

                        lNode->right = newLeft;
                        newNode->next = lNode->next;
                        
                        if ( lNode->next )
                        {
                                lNode->next->prev = newNode;
                        }
                        else
                        {
                                is->tail = newNode;
                        }

                        lNode->next = newNode;
                }
        }
        
        if ( should_print )
        {
                interval_set_print( is );
        }
}

int
main( int argc, char** argv )
{

        size_t                  bufSize = 1024;
        char*                   buf = ( char* ) malloc( bufSize );
        interval_set_t*         is = ( interval_set_t* ) calloc( 1, sizeof( interval_set_t ) );
        
        while (1)
        {
                printf( "Enter an operation ('Q' to quit): " );
                if ( fgets( buf, bufSize, stdin ) == NULL || strcmp( strtok( buf, "\n" ), "Q" ) == 0 )
                {
                        break;
                }

                char*                   operationArg = strtok( buf, " " );
                int                     leftArg = atoi( strtok( NULL, " " ) );
                int                     rightArg = atoi( strtok( NULL, " " ) );

                if ( strcmp( operationArg, "A" ) == 0 )
                {
                        interval_set_add( is, leftArg, rightArg, SHOULD_PRINT );
                }
                else if ( strcmp( operationArg, "R" ) == 0 )
                {
                        interval_set_remove( is, leftArg, rightArg, SHOULD_PRINT );
                }

                memset( buf, 0, bufSize );
        }

        interval_set_free( is );

        free( buf );

        return 0;
}

