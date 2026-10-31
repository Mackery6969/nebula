#pragma once

<<<<<<< HEAD
#include <revolution.h>
=======
#include <revolution/gx.h>
#include <revolution/mtx.h>
#include <revolution/os.h>
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main

class JKRHeap;

class OSThreadWrapper {
public:
    OSThreadWrapper(u32, int, int, JKRHeap*);

    virtual ~OSThreadWrapper();
    virtual void* run();

    void initMessageQueue(JKRHeap*, int);
    void initHeapSpecified(JKRHeap*, u32, int);
    static void* start(void*);

    /* 0x00 */ JKRHeap* mHeap;
<<<<<<< HEAD
    /* 0x04 */ OSThread* mThread;
    /* 0x08 */ OSMessageQueue mQueue;
    /* 0x2C */ OSMessage* mMessageArray;
=======
    /* 0x04 */ NebulaThread* mThread;
    /* 0x08 */ NebulaMessageQueue mQueue;
    /* 0x2C */ void** mMessageArray;
>>>>>>> upstream/main
    /* 0x30 */ u32 mMessageCount;
    /* 0x34 */ u8* mStack;
    /* 0x38 */ u32 mStackSize;
};
