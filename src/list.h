#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "error.h"

typedef int ListUnit;
typedef unsigned long ListIndex;

typedef struct {
    ListIndex capacity;
    bool isDoubleLinked;
    ListUnit*    data;
    ListIndex*   next;
    ListIndex*   prev;
    ListIndex    free;
    ListIndex    freeTail;
    Error status;
    bool initialized;
} List;

Error listInit(List* lst, size_t initialCapacity, bool isDoubleLinked);
List* listAlloc(size_t initialCapacity, bool isDoubleLinked, Error* status);

/// Note 1: If the list is single-linked, 
/// then make sure the passed in index is an actual existing element in the list
/// and not a part of the free area, otherwise some undefined behaviour may arise
/// Note 2: Does not support inserting after fictional 0th element if the list isnt empty
ListIndex  listAddAfter(List* lst, ListIndex index, ListUnit value, Error* status);
ListIndex  listAddAfterHead(List* lst, ListUnit value, Error* status);
ListIndex  listAddAfterTail(List* lst, ListUnit value, Error* status);
/// Note: any addBefore funcs do not support single-linked lists
ListIndex  listAddBefore(List* lst, ListIndex index, ListUnit value, Error* status);
ListIndex  listAddBeforeHead(List* lst, ListUnit value, Error* status);
ListIndex  listAddBeforeTail(List* lst, ListUnit value, Error* status);

/// Note: any delete funcs do not support single-linked lists
Error listDelete(List* lst, ListIndex index);
Error listDeleteHead(List* lst);
Error listDeleteTail(List* lst);

ListIndex  listGetHead(List* lst, Error* status);
ListIndex  listGetTail(List* lst, Error* status);
size_t     listGetCapacity(List* lst, Error* status);
ListIndex  listGetPrev(List* lst, ListIndex index, Error* status);
ListIndex  listGetNext(List* lst, ListIndex index, Error* status);
ListUnit   listGetValue(List* lst, ListIndex index, Error* status);
Error listSetValue(List* lst, ListIndex index, ListUnit value);

/// Note: O(n) time complexity
Error listLoopCheck(List* lst);
/// Note: O(n) time complexity
Error listLinearize(List* lst);

Error listDestroy(List* lst, bool isAlloced);
Error listVerify(List* lst);

#endif
