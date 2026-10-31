#pragma once

#include "Game/System/OSThreadWrapper.hpp"
<<<<<<< HEAD
=======
#include "Game/System/NebulaKernel.hpp"
>>>>>>> upstream/main
#include "Game/Util/Array.hpp"
#include "Game/Util/StringUtil.hpp"

class JKRExpHeap;
class JKRUnitHeap;

namespace MR {
    class FunctorBase;
};  // namespace MR

class FunctionAsyncExecInfo {
public:
    FunctionAsyncExecInfo(MR::FunctorBase*, int, const char*);

    ~FunctionAsyncExecInfo();

    void execute();

    bool isSame(const char* pStr) {
        return MR::strcasecmp(mName, pStr) == 0;
    }

    /* 0x00 */ MR::FunctorBase* mFunc;
    /* 0x04 */ int mPriority;
    /* 0x08 */ const char* mName;
    /* 0x0C */ volatile bool mIsEnd;
<<<<<<< HEAD
    /* 0x10 */ OSMessageQueue mQueue;
    /* 0x30 */ OSMessage mMessage;
=======
    /* 0x10 */ NebulaMessageQueue mQueue;
    /* 0x30 */ void* mMessage;
>>>>>>> upstream/main
};

class FunctionAsyncExecutorThread : public OSThreadWrapper {
public:
    FunctionAsyncExecutorThread(JKRHeap*);

    virtual void* run();

    /* 0x3C */ bool mIsSuspended;
    /* 0x40 */ const char* mName;
};

class FunctionAsyncExecutorOnMainThread {
public:
<<<<<<< HEAD
    FunctionAsyncExecutorOnMainThread(OSThread*);
=======
    FunctionAsyncExecutorOnMainThread(NebulaThread*);
>>>>>>> upstream/main

    void update();

    /* 0x000 */ bool _0;
<<<<<<< HEAD
    /* 0x004 */ OSMessageQueue mQueue;
    /* 0x024 */ OSMessage mMsgArray[64];
    /* 0x124 */ OSThread* mThread;
=======
    /* 0x004 */ NebulaMessageQueue mQueue;
    /* 0x024 */ void* mMsgArray[64];
    /* 0x124 */ NebulaThread* mThread;
>>>>>>> upstream/main
};

class FunctionAsyncExecutor {
public:
    FunctionAsyncExecutor();

    void update();
    void start(const MR::FunctorBase&, int, const char*);
    bool startOnMainThread(const MR::FunctorBase&, const char*);
    void waitForEnd(const char*);
    bool isEnd(const char*) const;
<<<<<<< HEAD
    OSThread* getOSThread(const char*);
=======
    NebulaThread* getOSThread(const char*);
>>>>>>> upstream/main
    FunctionAsyncExecInfo* createAndAddExecInfo(const MR::FunctorBase&, int, const char*);
    FunctionAsyncExecutorThread* getSuspendThread();

    /* 0x000 */ FunctionAsyncExecutorThread* mThreads[2];
    /* 0x008 */ FunctionAsyncExecutorOnMainThread* mMainThreadExec;
    /* 0x00C */ MR::Vector< MR::FixedArray< FunctionAsyncExecInfo*, 256 > > mHolders;
    /* 0x410 */ JKRUnitHeap* _410;
    /* 0x414 */ JKRExpHeap* _414;
};
