#include "list.h"
#include "dump.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    ListStatus status = OK;
    List* lst = listDynamicInit(5, true, &status);
    if (status)
        return status;

    FILE* logFile = initLogFile();

    listDump(logFile, lst, "<h3>inited</h3>");

    listAddBeforeHead(lst, 1);
    listAddBeforeTail(lst, 2);
    listAddBeforeTail(lst, 3);
    listDump(logFile, lst, "<h3>added elements</h3>");

    listAddBeforeHead(lst, 4);
    listAddBeforeTail(lst, 5);
    listAddBeforeTail(lst, 6);
    listDump(logFile, lst, "<h3>added elements</h3>");

    listDeleteTail(lst);
    listDump(logFile, lst, "<h3>deleted tail</h3>");

    listDeleteHead(lst);
    listDump(logFile, lst, "<h3>deleted head</h3>");

    listDelete(lst, 5);
    listAddBeforeHead(lst, 7);
    listAddBeforeTail(lst, 8);
    listAddBeforeTail(lst, 9);
    listAddBeforeHead(lst, 10);
    listAddBeforeTail(lst, 11);
    listDelete(lst, 4);
    listDelete(lst, 2);
    listAddBeforeTail(lst, 12);
    listAddBeforeHead(lst, 13);
    listAddBeforeTail(lst, 14);
    listAddBeforeTail(lst, 15);
    listDelete(lst, 6);
    listDelete(lst, 3);
    listAddBeforeHead(lst, 16);
    listAddBeforeTail(lst, 17);
    listAddBeforeTail(lst, 18);
    listDelete(lst, 6);
    listDelete(lst, 3);
    listDelete(lst, 1);
    listDump(logFile, lst, "<h3>monstrosity of a list</h3>");

    listLinearize(lst);
    listDump(logFile, lst, "<h3>all organized</h3>");

    fclose(logFile);
    listDestroy(lst, true);
    return 0;
}
