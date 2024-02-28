# AddRemoveIntervalsProblem

This repo houses my solution in C to the following problem, based on maintaining a valid set of disjoint integer intervals.

- [Problem Statement](#problem-statement)
- [Solution](#solution)
- [Build Instructions](#build-instructions)

## Problem Statement

Write a program that manages a disjoint set of integer intervals. 

E.g.: {[1, 3), [4, 6)} is a valid object that contains two intervals. 
{[1, 3), [3, 6)} is not a valid object because it is not disjoint. {[1, 6)} is the intended result.

Empty set {} means no interval, it is the default/start state.
 
Implement two functions:
 
add(newLeft, newRight)
remove(newLeft, newRight)
 
Here is an example sequence:
 ```
Start: {}
Call: add(1, 5)  =>  {[1, 5)}
Call: remove(2, 3) => {[1, 2), [3, 5)}
Call: add(6, 8) => {[1, 2), [3, 5), [6, 8)}
Call: remove(4, 7) => {[1, 2), [3, 4), [7, 8)}
Call: add(2, 7) => {[1, 8)}
 
etc.
```

## Solution

I decided that the best data structure to store the intervals as nodes is a linked list. From there we could devise an iterative way to add and remove intervals.

The solution could be done with a singly linked list, but we would have to manage 'previous' pointer a lot more in the implementations of the 'add' and 'remove' function during list traversals, so I chose a doubly linked list for convenience.

Each node contains a 'left' integer and 'right' integer, representing the interval range `[left, right)`, along with `next` and `prev` pointers.

The disjoint interval set can be represented in code as struct, which simply stores the head and tail of the linked list.

With that determined, adding or removing intervals is essentially just accounting for a large set of possible cases with respect to the interval set.

The key to determining which case we need to execute is to find out what intervals the new `left` or `right` values would be within. 

### For example:
```
    Given: { [1, 2), [5, 7), [10, 12), [15, 18) }
    Adding: [6, 11)
    Solution:
        We know that 6 is within the interval [5, 7) and 11 is within [10, 12). 
        The update we would make to the interval set is to combine those two 
        intervals to be [5, 12). 

        This would produce the following interval set:
        
            { [1, 2), [5, 12), [15, 18) }
```

If neither 'left' nor 'right' reside in any interval, that narrows down the solution to 4 subcases, which can be checked for by comparing the new 'left' or 'right' values to the nearest intervals in the set.

In total there should be 8 cases when adding or removing intervals. I don't believe there is a generic method to account for every case other than to hard code them. At some point in the future, I will collect and document every case in this read me, but for now the code can be referenced to account for them.

## Build Instructions

The solution can be built with gcc using a _one-liner_.

```
gcc -o solution ./intervals_solutions.c
```

And ran like so:
```
./solution
```

The program will continually prompt for an input, either "A" or "R" followed by two integers.
"A" will execute the `interval_set_add` function supplying the two integers you entered as the
left and right values of the interval you would like to add or remove from the set. Exit the 
program by forcing the process to quit ( CTRL-C ).

A sample execution would like this:
```
Enter an operation: A 1 2   // User entered 'A 1 2' here.
{[1, 2)}
Enter an operation: A 5 7
{[1, 2), [5, 7)}
Enter an operation: R 2 6
{[1, 7)}
```

The input doesn't check for bad input, so expect segmentation faults for anything diverging from
the above input format.

