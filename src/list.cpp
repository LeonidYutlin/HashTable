#include "listdef.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const ListUnit CANARY = (ListUnit)0xAAAbD0CC; //Appledog

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

static ListIndex  linkNewUnit(List* lst, ListIndex prev, ListIndex next, ListUnit value);
static ListStatus reallocateList(List* lst, size_t newCapacity);

ListStatus listInit(List* lst, size_t initialCapacity, bool isDoubleLinked) {
    ListStatus status = listVerify(lst);
    if (status != UninitializedList &&
        status != DestroyedList)
        return AttemptedReinitialization;
    if (initialCapacity == 0)
        return InvalidParameters;

    /// +1 for the fictional 0th element
    size_t actualCapacity = initialCapacity + 1;
    ListUnit* tempDataPtr =       (ListUnit*)calloc(actualCapacity, sizeof(ListUnit));
    ListIndex*   tempNextPtr =   (ListIndex*)calloc(actualCapacity, sizeof(ListIndex));
    ListIndex*   tempPrevPtr = isDoubleLinked
                               ? (ListIndex*)calloc(actualCapacity, sizeof(ListIndex))
                               : NULL;
    if (!tempDataPtr || !tempNextPtr || (isDoubleLinked && !tempPrevPtr)) {
        free(tempDataPtr); free(tempNextPtr); free(tempPrevPtr);
        return FailMemoryAllocation;
    }

    lst->data = tempDataPtr;
    lst->next = tempNextPtr;
    lst->prev = tempPrevPtr;
    lst->isDoubleLinked = isDoubleLinked;
    lst->free = 1;
    lst->status = OK;
    lst->capacity = actualCapacity;
    lst->data[0] = CANARY;

    for (ListIndex i = 2; i < actualCapacity; i++)
        lst->next[i - 1] = i;

    return listVerify(lst);
}

List* listDynamicInit(size_t initialCapacity, bool isDoubleLinked, ListStatus* status) {
    if (initialCapacity == 0)
        RETURN_WITH_STATUS(InvalidParameters, NULL);

    List* lst = (List*)calloc(1, sizeof(List));
    if (!lst)
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);

    *lst = {0};
    if (listInit(lst, initialCapacity, isDoubleLinked)) {
        free(lst);
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);
    }

    ListStatus returnedStatus = listVerify(lst);
    RETURN_WITH_STATUS(returnedStatus, lst);
}

#define IS_INVALID_INDEX(index) \
        (index > lst->capacity || \
         (lst->isDoubleLinked && index != lst->next[0] && lst->prev[index] == 0))

//diff name?
#define VERIFY_LIST() \
        ListStatus returnedStatus = listVerify(lst); \
        if (returnedStatus) \
            RETURN_WITH_STATUS(returnedStatus, 0);

ListIndex listAddAfter(List* lst, ListIndex index, ListUnit value, ListStatus* status) {
    VERIFY_LIST();
    if (IS_INVALID_INDEX(index))
        RETURN_WITH_STATUS(InvalidParameters, 0);

    if ((lst->free == 0) && (returnedStatus = reallocateList(lst, lst->capacity * 2 - 1)))
        RETURN_WITH_STATUS(returnedStatus, 0);

    return linkNewUnit(lst, index, lst->next[index], value);
}

ListIndex listAddBefore(List* lst, ListIndex index, ListUnit value, ListStatus* status) {
    VERIFY_LIST();
    if (!lst->isDoubleLinked)
        RETURN_WITH_STATUS(InvalidParameters, 0);
    if (IS_INVALID_INDEX(index))
        RETURN_WITH_STATUS(InvalidParameters, 0);

    if ((lst->free == 0) && (returnedStatus = reallocateList(lst, lst->capacity * 2 - 1)))
        RETURN_WITH_STATUS(returnedStatus, 0);

    return linkNewUnit(lst, lst->prev[index], index, value);
}

ListIndex listAddAfterTail(List* lst, ListUnit value, ListStatus* status) {
    VERIFY_LIST();

    return listAddAfter(lst, lst->prev[0], value, status);
}

ListIndex listAddAfterHead(List* lst, ListUnit value, ListStatus* status) {
    VERIFY_LIST();

    return listAddAfter(lst, lst->next[0], value, status);
}

ListIndex listAddBeforeTail(List* lst, ListUnit value, ListStatus* status) {
    VERIFY_LIST();

    return listAddBefore(lst, lst->prev[0], value, status);
}

ListIndex listAddBeforeHead(List* lst, ListUnit value, ListStatus* status) {
    VERIFY_LIST();

    return listAddBefore(lst, lst->next[0], value, status);
}

ListStatus listDelete(List* lst, ListIndex index) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;
    if (!lst->isDoubleLinked)
        return InvalidParameters;
    if (IS_INVALID_INDEX(index))
        return InvalidParameters;

    lst->next[lst->prev[index]] = lst->next[index];
    lst->prev[lst->next[index]] = lst->prev[index];
    lst->data[index] = 0;
    ListIndex oldFree = lst->free;
    lst->free = index;
    lst->next[index] = oldFree;
    lst->prev[index] = 0;

    return OK;
}

ListStatus listDeleteHead(List* lst) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;
    if (!lst->isDoubleLinked)
        return InvalidParameters;

    return listDelete(lst, lst->next[0]);
}

ListStatus listDeleteTail(List* lst) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;
    if (!lst->isDoubleLinked)
        return InvalidParameters;

    return listDelete(lst, lst->prev[0]);
}

#define FREE(ptr) \
        { \
        if (ptr) { \
            free(ptr); \
            ptr = NULL; \
        } \
        }

ListStatus listDestroy(List* lst, bool isAlloced) {
    if (!lst)
        return InvalidParameters;

    FREE(lst->data);
    FREE(lst->next);
    FREE(lst->prev);

    lst->capacity = 0;
    lst->status = DestroyedList;

    if (isAlloced)
        free(lst);

    return OK;
}

#undef FREE

ListIndex  listGetHead(List* lst, ListStatus* status) {
    VERIFY_LIST();

    return lst->next[0];
}

ListIndex  listGetTail(List* lst, ListStatus* status) {
    VERIFY_LIST();

    return lst->prev[0];
}

size_t listGetCapacity(List* lst, ListStatus* status) {
    VERIFY_LIST();

    return lst->capacity - 1;
}

ListIndex listGetPrev(List* lst, ListIndex index, ListStatus* status) {
    VERIFY_LIST();
    if (!lst->isDoubleLinked)
        RETURN_WITH_STATUS(InvalidParameters, 0);
    if (IS_INVALID_INDEX(index))
        RETURN_WITH_STATUS(InvalidParameters, 0);

    return lst->prev[index];
}

ListIndex listGetNext(List* lst, ListIndex index, ListStatus* status) {
    VERIFY_LIST();
    if (IS_INVALID_INDEX(index))
        RETURN_WITH_STATUS(InvalidParameters, 0);

    return lst->next[index];
}

ListUnit listGetValue(List* lst, ListIndex index, ListStatus* status) {
    VERIFY_LIST();
    if (IS_INVALID_INDEX(index))
        RETURN_WITH_STATUS(InvalidParameters, 0);

    return lst->data[index];
}

#undef VERIFY_LIST

ListStatus listSetValue(List* lst, ListIndex index, ListUnit value) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;
    if (IS_INVALID_INDEX(index))
        return InvalidParameters;

    lst->data[index] = value;

    return OK;
}

#define SOFT_ERROR(condition, status) \
        { \
        if (condition) \
            return status; \
        }

#define HARD_ERROR(condition, newStatus) \
        { \
        if (condition) \
            return (lst->status = newStatus); \
        }

ListStatus listVerify(List* lst) {
    SOFT_ERROR(!lst,                         InvalidParameters);
    SOFT_ERROR(lst->status,                  lst->status);
    HARD_ERROR(!lst->data,                   NullDataPointer);
    HARD_ERROR(!lst->next,                   NullNextPointer);
    HARD_ERROR(lst->isDoubleLinked &&
              !lst->prev,                    NullPrevPointer);
    HARD_ERROR(lst->prev[0] > lst->capacity, TailOutOfBounds);
    HARD_ERROR(lst->next[0] > lst->capacity, HeadOutOfBounds);
    HARD_ERROR(lst->free > lst->capacity,    FreeOutOfBounds);
    HARD_ERROR(lst->data[0] != CANARY,       CorruptedCanary);
    return lst->status;
}

#undef HARD_ERROR
#undef SOFT_ERROR

#define RETURN_IF_LOOPED(index) \
        { \
        if (beenVisited[cur]) { \
            free(beenVisited); \
            return (lst->status = LoopedConnections); \
        } \
        beenVisited[cur] = 1; \
        totalVisited++; \
        }

ListStatus listLoopCheck(List* lst) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;

    bool* beenVisited = (bool*)calloc(lst->capacity, sizeof(bool));
    size_t totalVisited = 0;
    if (lst->next[0]) {
        for (ListIndex cur = lst->next[0];
            lst->next[cur] < lst->capacity;
            cur = lst->next[cur]) {
            RETURN_IF_LOOPED(cur);
            if (!lst->next[cur])
                break;
        }
    }
    if (lst->free) {
        for (ListIndex cur = lst->free;
            lst->next[cur] < lst->capacity;
            cur = lst->next[cur]) {
            RETURN_IF_LOOPED(cur);
            if (!lst->next[cur])
                break;
        }
    }
    free(beenVisited);
    //beenVisited = NULL;

    return totalVisited == lst->capacity - 1
           ? OK
           : (lst->status = DanglingUnit);
}

#undef RETURN_IF_LOOPED

ListStatus listLinearize(List* lst) {
    ListStatus status = listVerify(lst);
    if (status)
        return status;

    List* temp = listDynamicInit(listGetCapacity(lst), lst->isDoubleLinked, &status);
    if (status) {
        listDestroy(temp, true);
        return status;
    }

    for (ListIndex cur = lst->next[0];
         lst->next[cur] < lst->capacity;
         cur = lst->next[cur]) {
        listAddAfterTail(temp, lst->data[cur], &status);
        if (status) {
            listDestroy(temp, true);
            return status;
        }
        if (!lst->next[cur])
            break;
    }

    for (ListIndex cur = 0;
         cur < temp->capacity;
         cur++) {
        lst->data[cur] = temp->data[cur];
        lst->next[cur] = temp->next[cur];
        if (lst->isDoubleLinked)
            lst->prev[cur] = temp->prev[cur];
    }
    lst->free = temp->free;

    listDestroy(temp, true);
    //temp = NULL;
    return listVerify(lst);
}

ListStatus listResize(List* lst, size_t newCapacity) {
    ListStatus returnedStatus = listVerify(lst);
    if (returnedStatus)
        return returnedStatus;

    /// for the fictional 0th element
    newCapacity++;
    if ( newCapacity <= lst->prev[0] ||
         newCapacity <= lst->next[0] ||
        (newCapacity < lst->capacity &&
         lst->isDoubleLinked &&
        (lst->prev[newCapacity] != 0)))
        return InvalidParameters;

    if (newCapacity == lst->capacity)
        return OK;

    return reallocateList(lst, newCapacity);
}

#define SET_REALLOCED(ptr, tempPtr) \
        { \
        if (!tempPtr) \
            return FailMemoryAllocation; \
        ptr = tempPtr; \
        }

static ListStatus reallocateList(List* lst, size_t newCapacity) {
    assert(lst);

    ListUnit* tempDataPtr = (ListUnit*)realloc(lst->data, newCapacity * sizeof(ListUnit));
    SET_REALLOCED(lst->data, tempDataPtr);
    ListIndex*   tempNextPtr =   (ListIndex*)realloc(lst->next, newCapacity * sizeof(ListIndex));
    SET_REALLOCED(lst->next, tempNextPtr);
    if (lst->isDoubleLinked) {
        ListIndex*   tempPrevPtr = (ListIndex*)realloc(lst->prev, newCapacity * sizeof(ListIndex));
        SET_REALLOCED(lst->prev, tempPrevPtr);
    }

    if (lst->free == 0) { //if no free units left
        lst->free = lst->capacity;
    } else {
        if (lst->free >= newCapacity) { //free is out of new bounds (edge cases will arise!)
            lst->free = 0;
        } else {
            ListIndex newFreeTailNext = newCapacity > lst->capacity ? lst->capacity : 0;
            //naive probing
            if (lst->next[lst->capacity - 1] == 0 &&
                lst->isDoubleLinked &&
                lst->prev[lst->capacity - 1] == 0) {
                lst->next[lst->capacity - 1] = newFreeTailNext;
            } else {
                //i had to do it... it's not O(1)
                //find first valid "tail" and bind it to new free elems or make it terminated
                //(edge cases will arise!)
                ListIndex freeTail = lst->free;
                while (lst->next[freeTail] != 0 && lst->next[freeTail] < newCapacity) {
                    ListIndex temp = lst->next[freeTail];
                    freeTail = temp;
                }
                lst->next[freeTail] = newFreeTailNext;
            }
        }
    }

    //initialize new free elements
    if (newCapacity > lst->capacity) {
        for (ListIndex i = lst->capacity; i < newCapacity; i++) {
            lst->data[i] = 0;
            if (lst->isDoubleLinked)
                lst->prev[i] = 0;
            if (i != lst->capacity) //dont do for the first iteration
                lst->next[i - 1] = i;
        }
        lst->next[newCapacity - 1] = 0;
    }

    lst->capacity = newCapacity;

    return OK;
}

#undef SET_REALLOCED

static ListIndex linkNewUnit(List* lst, ListIndex prev, ListIndex next, ListUnit value) {
    assert(lst);

    ListIndex center = lst->free;

    lst->free               = lst->next[center];
    lst->data[center]       = value;
    lst->next[center]       = next;
    lst->next[prev]         = center;
    if (lst->isDoubleLinked) {
        lst->prev[center]   = prev;
        lst->prev[next]     = center;
    }

    return center;
}

#undef RETURN_WITH_STATUS
#undef IS_INVALID_INDEX
