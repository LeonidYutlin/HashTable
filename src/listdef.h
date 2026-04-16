#ifndef LIST_DEF_H
#define LIST_DEF_H

#include "list.h"

struct List{
    ListIndex capacity = 0;
    bool isDoubleLinked = true;
    ListUnit*    data = NULL;
    ListIndex*   next = NULL;
    ListIndex*   prev = NULL;
    ListIndex    free = 0;
    ListStatus status = UninitializedList;
};

#endif
