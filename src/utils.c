#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <float.h>

#define TIMESTAMP_BUF_SZ 32

Error snTimestampedFilename(char* dest, size_t n, 
                            const char* prefix, 
                            const char* suffix, 
                            uint count) {
  if (!prefix || !suffix)
    return BadArgs;

  char tsBuf[TIMESTAMP_BUF_SZ] = {};
  Error err = OK;
  if ((err = snTimestamp(tsBuf, TIMESTAMP_BUF_SZ)))
    return err;

  if (count) {
    if (snprintf(dest, n, 
                 "%s%s-%u%s", 
                 prefix, tsBuf, 
                 count, suffix) <= 0)
      return LongFormat;
  } else {
    if (snprintf(dest, n, 
                 "%s%s%s", 
                 prefix, tsBuf,
                 suffix) <= 0)
      return LongFormat;
  }

  return OK;
}

Error snTimestamp(char* dest, size_t n) {
  time_t timeAbs = time(NULL);
  struct tm* localTime = localtime(&timeAbs);
  return strftime(dest, n, "%d-%m-%Y-%H:%M:%S", localTime)
         ? OK
         : LongFormat;
}
