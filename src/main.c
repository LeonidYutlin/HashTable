#include "list.h"
#include "dump.h"
#include "error.h"
#include <stdio.h>

#define new(str, size, hsh, val) (ListUnit){.key = (StringView){str, size}, .hash = hsh, .value = val}

#define SV(str) (StringView){ .data = str, .size = sizeof(str) - 1 }


uint64_t hashv1(StringView key);

uint64_t hashv1(StringView key) {
  return 0;
}

int main() {
  Error err = OK;
  List* lst = listAlloc(5, true, &err);
  if (err)
    return err;

  FILE* logFile = initLogFile();

  //listDump(logFile, lst, "<h3>inited</h3>");

  listAddBeforeHead(lst, new("Wawawa", 3, 56, 123), &err);
  listAddBeforeTail(lst, new("Apple", 5, 88, 4), &err);
  listAddBeforeTail(lst, new("Banana", 4, 100, 6), &err);
  //listDump(logFile, lst, "<h3>added elements</h3>");

  listAddBeforeHead(lst, new("here comes another one", 10, 80, 13), &err);
  listAddBeforeTail(lst, new("Awu", 3, 5, 23), &err);
  listAddBeforeTail(lst, new("uwa", 3, 6, 12), &err);
  //listDump(logFile, lst, "<h3>added elements</h3>");

  listDestroy(lst, true);

  HashTable* t = hashTableAlloc(5, 8, hashv1, &err);
  if (err)
    return err;
  
  hashTableDump(logFile, t, "<h2>alloced</h2>");

  hashTablePut(t, SV("The"), 1);
  hashTablePut(t, SV("Great"), 2);
  hashTablePut(t, SV("Gatsby"), 3);
  hashTablePut(t, SV("Is"), 4);
  hashTablePut(t, SV("A"), 5);
  hashTablePut(t, SV("Book"), 6);
  hashTablePut(t, SV("And"), 7);
  hashTablePut(t, SV("Yalishiduode"), 8);
  hashTablePut(t, SV("Is"), 9);
  hashTablePut(t, SV("A"), 10);
  hashTablePut(t, SV("Name"), 11);
  hashTablePut(t, SV("Of"), 12);
  hashTablePut(t, SV("A"), 9999);
  hashTablePut(t, SV("Cat"), 14);

  listDump(logFile, t->buckets, "hey");
  hashTableDump(logFile, t, "<h2>Added elements</h2>");

  hashTableDelete(t, SV("Cat"));

  hashTableDump(logFile, t, "<h2>Deleted Cat</h2>");

  uint64_t v = hashTableGet(t, SV("Yalishiduode"), &err);
  if (err)
    return err;
  printf("Yalishiduode is %lu\n", v);

  hashTableDestroy(t, true);
  fclose(logFile);
  return 0;
}
