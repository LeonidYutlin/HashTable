#ifndef ERROR_H
#define ERROR_H

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
} Error;



#endif
