#include "list.h"
#include "dump.h"
#include "error.h"
#include <stdio.h>

int main() {
  Error err = OK;
  List* lst = listAlloc(5, true, &err);
  if (err)
    return err;

  FILE* logFile = initLogFile();

  listDump(logFile, lst, "<h3>inited</h3>");

  listAddBeforeHead(lst, 1, &err);
  listAddBeforeTail(lst, 2, &err);
  listAddBeforeTail(lst, 3, &err);
  listDump(logFile, lst, "<h3>added elements</h3>");

  listAddBeforeHead(lst, 4, &err);
  listAddBeforeTail(lst, 5, &err);
  listAddBeforeTail(lst, 6, &err);
  listDump(logFile, lst, "<h3>added elements</h3>");
 
  listDeleteTail(lst);
  listDump(logFile, lst, "<h3>deleted tail</h3>");

  listDeleteHead(lst);
  listDump(logFile, lst, "<h3>deleted head</h3>");

  listDelete(lst, 5);
  listAddBeforeHead(lst, 7, &err);
  listAddBeforeTail(lst, 8, &err);
  listAddBeforeTail(lst, 9, &err);
  listAddBeforeHead(lst, 10, &err);
  listAddBeforeTail(lst, 11, &err);
  listDelete(lst, 4);
  listDelete(lst, 2);
  listAddBeforeTail(lst, 12, &err);
  listAddBeforeHead(lst, 13, &err);
  listAddBeforeTail(lst, 14, &err);
  listAddBeforeTail(lst, 15, &err);
  listDelete(lst, 6);
  listDelete(lst, 3);
  listAddBeforeHead(lst, 16, &err);
  listAddBeforeTail(lst, 17, &err);
  listAddBeforeTail(lst, 18, &err);
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
