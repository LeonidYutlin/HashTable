#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

enum ListStatus {
    OK = 0,
    UninitializedList,
    DestroyedList,
    AttemptedReinitialization,
    InvalidParameters,
    FailMemoryAllocation,
    NullDataPointer,
    NullNextPointer,
    NullPrevPointer,
    CorruptedCanary,
    TailOutOfBounds,
    HeadOutOfBounds,
    FreeOutOfBounds,
    LoopedConnections,
    DanglingUnit,
};

typedef int ListUnit;
typedef unsigned long ListIndex;

struct List;

ListStatus listInit(List* lst, size_t initialCapacity, bool isDoubleLinked = true);
List*      listDynamicInit(size_t initialCapacity, bool isDoubleLinked = true,
                           ListStatus* status = NULL);

/// Note 1: If the list is single-linked, then make sure the passed in index is an actual existing element in the list
/// and not a part of the free area, otherwise some undefined behaviour may arise
/// Note 2: Does not support inserting after fictional 0th element if the list isnt empty
ListIndex  listAddAfter(List* lst, ListIndex index, ListUnit value, ListStatus* status = NULL);
ListIndex  listAddAfterHead(List* lst, ListUnit value, ListStatus* status = NULL);
ListIndex  listAddAfterTail(List* lst, ListUnit value, ListStatus* status = NULL);
/// Note: any addBefore funcs do not support single-linked lists
ListIndex  listAddBefore(List* lst, ListIndex index, ListUnit value, ListStatus* status = NULL);
ListIndex  listAddBeforeHead(List* lst, ListUnit value, ListStatus* status = NULL);
ListIndex  listAddBeforeTail(List* lst, ListUnit value, ListStatus* status = NULL);

/// Note: any delete funcs do not support single-linked lists
ListStatus listDelete(List* lst, ListIndex index);
ListStatus listDeleteHead(List* lst);
ListStatus listDeleteTail(List* lst);

ListIndex  listGetHead(List* lst, ListStatus* status = NULL);
ListIndex  listGetTail(List* lst, ListStatus* status = NULL);
size_t     listGetCapacity(List* lst, ListStatus* status = NULL);
ListIndex  listGetPrev(List* lst, ListIndex index, ListStatus* status = NULL);
ListIndex  listGetNext(List* lst, ListIndex index, ListStatus* status = NULL);
ListUnit   listGetValue(List* lst, ListIndex index, ListStatus* status = NULL);
ListStatus listSetValue(List* lst, ListIndex index, ListUnit value);
//ListStatus listGetStatus... thats quite literally listVerify

/// Note 1: linear-time complexity (O(how much unused units list has)
/// Note 2: impossible to check for 100%-valid newCapacity
///         without making this even more time complex, be cautious
ListStatus listResize(List* lst, size_t newCapacity);
/// Note: linear-time complexity
ListStatus listLoopCheck(List* lst);
/// Note: linear-time complexity
ListStatus listLinearize(List* lst);



ListStatus listDestroy(List* lst, bool isAlloced = false);
ListStatus listVerify(List* lst);

#endif
