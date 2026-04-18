#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include "list.h"

/*
Accounted situations:
    -Premature next 0
    -Postmature prev 0
    -next/prev link to out-of-capacity non-existing-node
    -next/prev link from main to free element
    -next/prev link from free to main element
    -next/prev mixup in main (wrongly linked, no back-to-back linking)
    -prev at head isnt 0
    -next at tail isnt 0
*/
Error listDump_(FILE* f, List* lst, const char* commentary, 
                const char* filename, int line);

#define listDump(f, lst, commentary) \
        listDump_(f, lst, commentary, __FILE__, __LINE__)

FILE* initLogFile();

#endif
