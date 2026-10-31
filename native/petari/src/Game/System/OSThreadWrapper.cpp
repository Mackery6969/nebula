#include "Game/System/OSThreadWrapper.hpp"
<<<<<<< HEAD
=======
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main
#include <JSystem/JKernel/JKRHeap.hpp>

OSThreadWrapper::OSThreadWrapper(u32 stackSize, int messageCount, int priority, JKRHeap* pHeap) {
    if (pHeap == nullptr) {
        pHeap = JKRHeap::sCurrentHeap;
    }

    initHeapSpecified(pHeap, stackSize, priority);
    initMessageQueue(mHeap, messageCount);
}

OSThreadWrapper::~OSThreadWrapper() {
    if (mHeap != nullptr) {
<<<<<<< HEAD
        if (!OSIsThreadTerminated(mThread)) {
            OSDetachThread(mThread);
            OSCancelThread(mThread);
=======
        if (!NebulaKernel::isTerminated(mThread)) {
            NebulaKernel::detach(mThread);
            NebulaKernel::cancel(mThread);
>>>>>>> upstream/main
        }

        delete[] mStack;
        delete mThread;
    }

    delete[] mMessageArray;
}

void OSThreadWrapper::initMessageQueue(JKRHeap* pHeap, int messageCount) {
    mMessageCount = messageCount;
<<<<<<< HEAD
    mMessageArray = new (pHeap, 0) OSMessage[mMessageCount];

    OSInitMessageQueue(&mQueue, mMessageArray, mMessageCount);
=======
    mMessageArray = new (pHeap, 0) void*[mMessageCount];

    mQueue.init(mMessageArray, mMessageCount);
>>>>>>> upstream/main
}

void OSThreadWrapper::initHeapSpecified(JKRHeap* pHeap, u32 stackSize, int priority) {
    mHeap = pHeap;
    mStackSize = stackSize & ~31;
    mStack = new (mHeap, 32) u8[mStackSize];
<<<<<<< HEAD
    mThread = new (mHeap, 32) OSThread();

    OSCreateThread(mThread, &OSThreadWrapper::start, this, &mStack[mStackSize], mStackSize, priority, 1 /* OS_THREAD_ATTR_DETACH */);
=======
    mThread = new (mHeap, 32) NebulaThread();

    NebulaKernel::create(mThread, &OSThreadWrapper::start, this, mStackSize, priority, true /* detached */);
>>>>>>> upstream/main
}

void* OSThreadWrapper::start(void* pArg) {
    return static_cast< OSThreadWrapper* >(pArg)->run();
}

void* OSThreadWrapper::run() {
    return nullptr;
}
