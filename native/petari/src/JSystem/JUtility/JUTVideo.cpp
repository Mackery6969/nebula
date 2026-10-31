#include "JSystem/JUtility/JUTVideo.hpp"
<<<<<<< HEAD
#include "JSystem/JUtility/JUTDirectPrint.hpp"
#include "JSystem/JUtility/JUTXfb.hpp"

JUTVideo* JUTVideo::sManager;

OSTick JUTVideo::sVideoLastTick;

OSTick JUTVideo::sVideoInterval;

static bool data_80451544;
=======
#include "Game/System/NebulaKernel.hpp"
#include <JSystem/J3DGraphBase/J3DNativeState.hpp>
#include "Game/System/NebulaFrameClock.hpp"

JUTVideo* JUTVideo::sManager;


>>>>>>> upstream/main

JUTVideo* JUTVideo::createManager(_GXRenderModeObj const* param_0) {
    if (sManager == nullptr) {
        sManager = new JUTVideo(param_0);
    }
    return sManager;
}

void JUTVideo::destroyManager() {
    if (sManager != nullptr) {
        delete sManager;
        sManager = nullptr;
    }
}

JUTVideo::JUTVideo(GXRenderModeObj const* param_0) {
    mRenderObj = nullptr;
<<<<<<< HEAD
    VIInit();
    mSetBlack = true;
    mSetBlackFrameCount = 2;
    setRenderMode(param_0);
    VISetBlack(1);
    VIFlush();
    field_0x8 = 0;
    mRetraceCount = VIGetRetraceCount();
    field_0x10 = 1;
    field_0x18 = 0;
    sVideoLastTick = OSGetTick();
    sVideoInterval = 670000;
    mPreRetraceCallback = VISetPreRetraceCallback(preRetraceProc);
    mPostRetraceCallback = VISetPostRetraceCallback(postRetraceProc);
    mPreCallback = nullptr;
    mPostCallback = nullptr;
    OSInitMessageQueue(&mMessageQueue, &mMessage, 1);
    GXSetDrawDoneCallback(drawDoneCallback);
}

JUTVideo::~JUTVideo() {
    VISetPreRetraceCallback(mPreRetraceCallback);
    VISetPostRetraceCallback(mPostRetraceCallback);
=======
    NebulaFrameClock::init();
    mSetBlack = true;
    mSetBlackFrameCount = 2;
    setRenderMode(param_0);
    NebulaFrameClock::show(false);
    field_0x8 = 0;
    mRetraceCount = NebulaFrameClock::count();
    field_0x10 = 1;
    field_0x18 = 0;
    mPreRetraceCallback = NebulaFrameClock::setPreHook(preRetraceProc);
    mPostRetraceCallback = NebulaFrameClock::setPostHook(postRetraceProc);
    mPreCallback = nullptr;
    mPostCallback = nullptr;
    mMessageQueue.init(&mMessage, 1);
}

JUTVideo::~JUTVideo() {
    NebulaFrameClock::setPreHook(mPreRetraceCallback);
    NebulaFrameClock::setPostHook(mPostRetraceCallback);
>>>>>>> upstream/main
}

void JUTVideo::preRetraceProc(u32 retrace_count) {
    if (sManager->mPreCallback) {
        (*sManager->mPreCallback)(retrace_count);
    }

<<<<<<< HEAD
    OSTick tick = OSGetTick();
    sVideoInterval = tick - sVideoLastTick;
    sVideoLastTick = tick;

    JUTXfb* xfb = JUTXfb::sManager;
    if (!xfb) {
        VISetBlack(TRUE);
        VIFlush();
        return;
    }

    static void* frameBuffer = nullptr;

    if (frameBuffer) {
        JUTVideo* videoManager = JUTGetVideoManager();
        const GXRenderModeObj* renderMode = videoManager->getRenderMode();
        JUTDirectPrint* directPrint = JUTDirectPrint::getManager();
        directPrint->changeFrameBuffer(frameBuffer, renderMode->fbWidth, renderMode->efbHeight);
    }

=======
    // The picture shown is the renderer's newest; black for the first retraces.
>>>>>>> upstream/main
    if (sManager->mSetBlack == 1) {
        s32 frame_count = sManager->mSetBlackFrameCount;
        if (frame_count > 0) {
            frame_count--;
        }

        sManager->mSetBlackFrameCount = frame_count;
        sManager->mSetBlack = frame_count != 0;
<<<<<<< HEAD
        VISetBlack(TRUE);
        VIFlush();
        return;
    }

    if (!xfb) {
        VISetBlack(TRUE);
        VIFlush();
        return;
    }

    u32 bufferNum = xfb->getBufferNum();
    if (bufferNum == 2 || bufferNum == 3) {
        if (!data_80451544) {
            s16 index = xfb->getDrawnXfbIndex();
            xfb->setDisplayingXfbIndex(index);
            if (index < 0) {
                VISetBlack(1);
                VIFlush();
            } else {
                VISetNextFrameBuffer(xfb->getDisplayingXfb());
                VIFlush();
                VISetBlack(FALSE);
                frameBuffer = xfb->getDisplayingXfb();
            }
        }
    } else if (xfb->getBufferNum() == 1) {
        if (xfb->getSDrawingFlag() == 0) {
            s16 index = xfb->getDrawnXfbIndex();
            if (index >= 0) {
                xfb->setDisplayingXfbIndex(index);
                GXCopyDisp(xfb->getDisplayingXfb(), GX_TRUE);
                GXFlush();
                xfb->setSDrawingFlag(2);
                frameBuffer = xfb->getDisplayingXfb();
                if (VIGetNextFrameBuffer()) {
                    VISetBlack(FALSE);
                }
            } else {
                VISetBlack(TRUE);
            }
        }
        VIFlush();
    }
}

void JUTVideo::drawDoneStart() {
    data_80451544 = true;
    GXSetDrawDone();
}

void JUTVideo::dummyNoDrawWait() {
    data_80451544 = false;
}

void JUTVideo::drawDoneCallback() {
    JUTXfb* xfb = JUTXfb::sManager;
    if (!xfb) {
        return;
    }

    data_80451544 = false;
    if (xfb->getBufferNum() == 1) {
        if (xfb->getSDrawingFlag() == 1) {
            xfb->setSDrawingFlag(0);

            void* frameBuffer = xfb->getDrawnXfb();
            if (frameBuffer) {
                VISetNextFrameBuffer(xfb->getDrawnXfb());
                VIFlush();
            }
        }
    }
=======
        NebulaFrameClock::show(false);
        return;
    }
    NebulaFrameClock::show(true);
>>>>>>> upstream/main
}

void JUTVideo::postRetraceProc(u32 retrace_count) {
    if (sManager->mPostCallback != nullptr) {
        sManager->mPostCallback(retrace_count);
    }

<<<<<<< HEAD
    OSMessage message = reinterpret_cast< OSMessage >(VIGetRetraceCount());
    OSSendMessage(&sManager->mMessageQueue, message, OS_MESSAGE_NOBLOCK);
=======
    void* message = reinterpret_cast< void* >(NebulaFrameClock::count());
    sManager->mMessageQueue.send(message, false);
>>>>>>> upstream/main
}

void JUTVideo::setRenderMode(GXRenderModeObj const* pObj) {
    if (mRenderObj != nullptr && pObj->viTVmode != mRenderObj->viTVmode) {
        mSetBlack = true;
        mSetBlackFrameCount = 4;
    }
    mRenderObj = const_cast< GXRenderModeObj* >(pObj);
<<<<<<< HEAD
    VIConfigure(mRenderObj);
    VIFlush();

    if (mSetBlack) {
        VIWaitForRetrace();
        VIWaitForRetrace();
=======

    if (mSetBlack) {
        NebulaFrameClock::waitFrame();
        NebulaFrameClock::waitFrame();
>>>>>>> upstream/main
    }
}

void JUTVideo::waitRetraceIfNeed() {
}
