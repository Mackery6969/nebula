#pragma once

<<<<<<< HEAD
#include <revolution/os/OSMutex.h>

class JASMutexLock {
public:
    inline JASMutexLock(OSMutex* mutex) {
        mMutex = mutex;
        OSLockMutex(mMutex);
    }
    inline ~JASMutexLock() {
        OSUnlockMutex(mMutex);
    }

    /* 0x0 */ OSMutex* mMutex;
=======
#include "Game/System/NebulaKernel.hpp"
#include "Game/System/NebulaKernel.hpp"

class JASMutexLock {
public:
    inline JASMutexLock(NebulaMutex* mutex) {
        mMutex = mutex;
        mMutex->lock();
    }
    inline ~JASMutexLock() {
        mMutex->unlock();
    }

    /* 0x0 */ NebulaMutex* mMutex;
>>>>>>> upstream/main
};
