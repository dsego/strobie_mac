/*
    Copyright (c) 2013 Davorin Šego. All rights reserved.
*/


#include <libkern/OSAtomic.h>

#define FullMemoryBarrier()  OSMemoryBarrier()

#define CASB(a,b,c)   OSAtomicCompareAndSwapIntBarrier(a, b, c);
