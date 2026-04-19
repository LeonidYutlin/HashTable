#ifndef ERROR_H
#define ERROR_H

#define RETURN_WITH_STATUS(value, returnValue)     \
  {                                                \
    if (status)                                    \
      *status = value;                             \
    return returnValue;                            \
  }

typedef enum {
    OK = 0,
    Fail,
    Uninitialized,
    Destroyed,
    DenyReinit,
    BadArgs,
    FailMemoryAllocation,
    FailMemoryReallocation,
    NullPointerField,
    CorruptedCanary,
    BadIndex,
    LoopedConnections,
    DanglingUnit,
    LongFormat,
    BadSize,
    NotFound,
} Error;

#endif
