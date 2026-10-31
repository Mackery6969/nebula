#pragma once

#include <revolution/os/OSInterrupt.h>
<<<<<<< HEAD

class JASCriticalSection {
public:
    JASCriticalSection() : mInterruptState(OSDisableInterrupts()) {};
    ~JASCriticalSection() {
        OSRestoreInterrupts(mInterruptState);
=======
#include "Game/System/NebulaKernel.hpp"

class JASCriticalSection {
public:
    JASCriticalSection() : mInterruptState(NebulaKernel::disableInterrupts()) {};
    ~JASCriticalSection() {
        NebulaKernel::restoreInterrupts(mInterruptState);
>>>>>>> upstream/main
    }
    /* 0x00 */ BOOL mInterruptState;
};
