#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include "error.h"

/// prints up to n chars to dest, in a form of 
/// <prefix><timestamp><?-count><suffix>
///  | <?-count> - if count != 0, then "-%d"
///    else the count is not printed
///  | <timestamp> - see snTimestamp(...)
Error snTimestampedFilename(char* dest, size_t n, 
                            const char* prefix, 
                            const char* suffix, 
                            uint count);

/// prints up to n chars to dest, in a form of 
/// "%day-%month-%Year-%Hour:%Minute:%Second"
Error snTimestamp(char* dest, size_t n);

#endif
