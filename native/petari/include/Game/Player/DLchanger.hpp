#pragma once

<<<<<<< HEAD
#include <JSystem/JKernel/JKRHeap.hpp>
=======
#include <JSystem/J3DGraphBase/J3DStateList.hpp>
>>>>>>> upstream/main
#include <revolution/types.h>

class J3DModelX;

struct DLholder {
<<<<<<< HEAD
    /* 0x00 */ u8* mDL;
    /* 0x04 */ u16 mSize;
    /* 0x06 */ u16 _6;
=======
    J3DStateList mList;
>>>>>>> upstream/main
};

class DLchanger {
public:
<<<<<<< HEAD
    DLchanger(u8 count, u32 size) {
=======
    // `capacity`: the changes each buffer holds.
    DLchanger(u8 count, u16 capacity) {
>>>>>>> upstream/main
        mBuffers = new DLholder[count];
        mBufferCount = count;
        mCurrentBuffer = 0;
        for (u32 i = 0; i < mBufferCount; i++) {
<<<<<<< HEAD
            mBuffers[i].mDL = new (32) u8[size];
=======
            mBuffers[i].mList.reserve(capacity);
>>>>>>> upstream/main
        }
    }

    void addDL(J3DModelX*);

    DLholder* swap() {
        mCurrentBuffer = (mCurrentBuffer + 1) % mBufferCount;
        return &mBuffers[mCurrentBuffer];
    }

    /* 0x00 */ DLholder* mBuffers;
    /* 0x04 */ u8 mBufferCount;
    /* 0x05 */ u8 mCurrentBuffer;
};
