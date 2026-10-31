#include "JSystem/JAudio2/JASReport.hpp"
<<<<<<< HEAD
#include "JSystem/JAudio2/JASCalc.hpp"
#include "JSystem/JKernel/JKRHeap.hpp"
#include <revolution/os/OSThread.h>
#include <cstdio>
#include <revolution/os/OSMutex.h>

static OSMutex sMutex;
=======
#include "Game/System/NebulaKernel.hpp"
#include "JSystem/JAudio2/JASCalc.hpp"
#include "JSystem/JKernel/JKRHeap.hpp"
#include "Game/System/NebulaKernel.hpp"
#include <cstdio>
#include "Game/System/NebulaKernel.hpp"

static NebulaMutex sMutex;
>>>>>>> upstream/main
static char* sBuffer;
static int sLineMax;
static int sLineCount;
static int sTop;

void JASReport(const char* pFormat, ...) {
    va_list vl;
    if (sBuffer == nullptr) {
        return;
    }

    va_start(vl, pFormat);
<<<<<<< HEAD
    OSMutex* pMutex = &sMutex;
    OSLockMutex(pMutex);
=======
    NebulaMutex* pMutex = &sMutex;
    pMutex->lock();
>>>>>>> upstream/main
    vsnprintf(sBuffer + (sTop * 64), 64, pFormat, vl);
    va_end(vl);

    sTop++;
    if (sTop >= sLineMax) {
        sTop = 0;
    }

    if (sLineCount < sLineMax) {
        sLineCount++;
    }

<<<<<<< HEAD
    OSUnlockMutex(pMutex);
=======
    pMutex->unlock();
>>>>>>> upstream/main
}
