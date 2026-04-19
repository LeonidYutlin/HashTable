#include "list.h"
#include "dump.h"
#include "error.h"
#include <stdio.h>

#define new(str, size, hsh, val) (ListUnit){.key = (StringView){str, size}, .hash = hsh, .value = val}

int main() {
  Error err = OK;
  List* lst = listAlloc(5, true, &err);
  if (err)
    return err;

  FILE* logFile = initLogFile();

  listDump(logFile, lst, "<h3>inited</h3>");

  listAddBeforeHead(lst, new("Wawawa", 3, 56, 123), &err);
  listAddBeforeTail(lst, new("Apple", 5, 88, 4), &err);
  listAddBeforeTail(lst, new("Banana", 4, 100, 6), &err);
  listDump(logFile, lst, "<h3>added elements</h3>");

  listAddBeforeHead(lst, new("here comes another one", 10, 80, 13), &err);
  listAddBeforeTail(lst, new("Awu", 3, 5, 23), &err);
  listAddBeforeTail(lst, new("uwa", 3, 6, 12), &err);
  listDump(logFile, lst, "<h3>added elements</h3>");

  fclose(logFile);
  listDestroy(lst, true);
  return 0;
}
