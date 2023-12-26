#pragma once


#include "string.h"
typedef unsigned char CLR_UINT8;
typedef signed int CLR_INT32;

#define BUF_MIN(x, y) ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y) ((x) > (y) ? (x) : (y))
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

typedef struct CircularBuffer
{
    CLR_UINT8 *buffer;
    CLR_INT32 size;
    CLR_INT32 r;
    CLR_INT32 w;
} CircularBuffer_t;


CLR_UINT8 InitializeCircularBuffer(CircularBuffer_t *buff, void *buffdata, CLR_INT32 size);
CLR_INT32 CircularBufferBytesWaiting(CircularBuffer_t *buff);
CLR_INT32 WriteCircularBuffer(CircularBuffer_t *buff, const void *data, CLR_INT32 btw);
CLR_INT32 ReadCircularBuffer(CircularBuffer_t *buff, void *data, CLR_INT32 btr);
